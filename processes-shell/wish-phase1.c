/*
    Phase 1

    No "path" builtin, no redirection, no parallel commands.

    Handles shell scripts: 

        $ ./wish my_script

    Handles "exit" and "cd" builtins.

    Runs one command in foreground:

        wish> ls           # tries to exec "ls" (fails), then "/bin/ls" (succeeds)
        wish> /bin/ls      # tries to exec "/bin/ls" (succeeds)

    Passes automated tests 1-6, 12-15, and 21.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

#ifdef DBG
#define DEBUG 1
#else
#define DEBUG 0
#endif

char errmsg[30] = "An error has occurred\n";
char *xargv[10];
int xargc;
bool is_builtin(char *cmdname);
void do_builtin(int argc, char *argv[]);
void parse_cmd(char *cmd);
int do_cmd(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    int r;
    char *line = NULL;
    size_t cap = 0;
    FILE *fp = stdin;
    bool interactive = true;
    pid_t child_pid;

    if (argc > 2)
    {
        fprintf(stderr, "%s", errmsg);
        exit(1);
    }

    if (argc == 2)
    {
        interactive = false;
        fp = fopen(argv[1], "r");
        if (fp == NULL)
        {
            fprintf(stderr, "%s", errmsg);
            exit(1);
        }
    }

    while (1)
    {
        if (interactive)
            printf("wish> ");
        r = getline(&line, &cap, fp);
        if (r == -1)
        {
            // Got EOF
            exit(0);
        }

        parse_cmd(line);
        if (xargc == 0)
        {
            continue;
        }
        if (is_builtin(xargv[0]))
            do_builtin(xargc, xargv);
        else
        {
            r = do_cmd(xargc, xargv);
            child_pid = r;
            waitpid(child_pid, NULL, 0);
        }
    }
}

bool is_builtin(char *cmdname)
{
    if (strcmp(cmdname, "exit") == 0)
        return true;
    if (strcmp(cmdname, "cd") == 0)
        return true;
    return false;
}

void do_builtin(int argc, char *argv[])
{
    if (strcmp(argv[0], "exit") == 0)
    {
        if (argc > 1)
            fprintf(stderr, "%s", errmsg);
        else
            exit(0);
    }
    if (strcmp(argv[0], "cd") == 0)
    {
        if (argc != 2)
            fprintf(stderr, "%s", errmsg);
        else if (chdir(argv[1]) < 0)
            fprintf(stderr, "%s", errmsg);
    }
}


void parse_cmd(char *cmd)
{
    if (DEBUG)
    {
        printf("cmd:%s\n", cmd);
    }

    // Parse the cmd into whitespace-separated args, put
    // args in xargv, number of args in argc
    xargc = 0;
    char **p;
    p = &xargv[xargc];
    while (true) {
        *p = strsep(&cmd, " \t\n");
        if (*p == NULL)
            break;
        if (**p != '\0') // first char in remaining cmd was a delimiter
        {
            xargc++;
            p = &xargv[xargc];
        }
    }

    if (DEBUG)
    {
        for (int i = 0; i < 10; i++)
        {
            if (xargv[i] == NULL)
                break;
            printf("xargv[%d]:%s\n", i, xargv[i]);
        }
        printf("xargc=%d\n", xargc);
    }
}

int do_cmd(int argc, char *argv[])
{
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        fprintf(stderr, "%s", errmsg);
        return pid;
    }
    if (pid == 0)
    {
        // child
        if (execv(argv[0], argv) < 0)
        {
            char *s = malloc(strlen("/bin/") + strlen(argv[0]) + 1);
            strcpy(s, "/bin/");
            strcat(s, argv[0]);
            if (DEBUG)
                printf("trying %s\n", s);
            execv(s, argv);

            // if we get here, all execs must have failed
            fprintf(stderr, "%s", errmsg);
            if (DEBUG)
                printf("tried cmd and /bin/cmd, all execs failed\n");
            exit(1);
        }
        return 0; // never get here; placate compiler
    }
    else
    {
        // parent
        return pid;
    }
}
