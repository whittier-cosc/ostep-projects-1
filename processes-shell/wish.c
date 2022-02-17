#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef DBG
#define DEBUG 1
#else
#define DEBUG 0
#endif

char errmsg[30] = "An error has occurred\n";
#define NPATH 10
char *path[NPATH];
int pathlen;
char *xargv[10];
int xargc;
char *cmdv[10];
int cmdc;
int pids[10];
int pidc;
char *redir;
void parse_line(char *line);
void parse_cmd(char *cmd);
bool is_builtin(char *cmdname);
void do_builtin(int argc, char *argv[]);
int do_cmd(int argc, char *argv[], char *redir);

int main(int argc, char *argv[])
{
    int r;
    char *line = NULL;
    size_t cap = 0;
    FILE *fp = stdin;
    bool interactive = true;

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

    path[0] = strdup("/bin");
    pathlen = 1;

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

        parse_line(line);
        if (cmdc == 0)
            continue;

        pidc = 0;
        for (int i = 0; i < cmdc; i++)
        {
            parse_cmd(cmdv[i]);
            if (xargc == 0)
            {
                // either cmd was whitespace only, or first non-space was '>'
                if (redir)
                {
                    fprintf(stderr, "%s", errmsg);
                }
                continue;
            }
            if (is_builtin(xargv[0]))
                do_builtin(xargc, xargv);
            else
            {
                r = do_cmd(xargc, xargv, redir);
                if (r > 0)
                {
                    pids[pidc++] = r;
                }
            }
        }
        for (int i = 0; i < pidc; i++)
        {
            waitpid(pids[i], NULL, 0);
        }
    }
}

bool is_builtin(char *cmdname)
{
    if (strcmp(cmdname, "exit") == 0)
        return true;
    if (strcmp(cmdname, "cd") == 0)
        return true;
    if (strcmp(cmdname, "path") == 0)
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
    if (strcmp(argv[0], "path") == 0)
    {
        for (int i = 1; i < argc; i++)
        {
            path[i - 1] = strdup(argv[i]);
        }
        pathlen = argc - 1;
        if (DEBUG)
        {
            printf("path set to ");
            for (int i = 0; i < pathlen; i++)
            {
                printf("%s ", path[i]);
            }
            printf("\n");
        }
    }
}

void parse_line(char *line)
{
    // cmd1 & cmd2 & ... & cmdn
    // split on '&', put cmds in cmdv, number of cmds in cmdc
    cmdc = 0;
    for (char **ap = cmdv; (*ap = strsep(&line, "&")) != NULL;)
    {
        if (**ap != '\0')
        {
            cmdc++;
            if (++ap >= &cmdv[10])
                break;
        }
    }
    if (DEBUG)
    {
        for (int i = 0; i < 10; i++)
        {
            if (cmdv[i] == NULL)
                break;
            printf("%d:%s\n", i, cmdv[i]);
        }
        printf("cmdc=%d\n", cmdc);
    }
}

void parse_cmd(char *cmd)
{
    // Look for redirection operator (>)
    redir = NULL;
    for (int i = 0, n = strlen(cmd); i < n; i++)
    {
        if (cmd[i] == '>')
        {
            redir = malloc(n - i);
            strcpy(redir, cmd + i + 1);
            cmd[i] = '\0';
            break;
        }
    }

    if (DEBUG)
    {
        printf("cmd:%s\n", cmd);
        if (redir)
            printf("redir:%s", redir);
    }

    // Parse the cmd into whitespace-separated args, put
    // args in xargv, number of args in argc
    xargc = 0;
    for (char **ap = xargv; (*ap = strsep(&cmd, " \t\n")) != NULL;)
    {
        if (**ap != '\0')
        {
            xargc++;
            if (++ap >= &xargv[10])
                break;
        }
    }

    if (DEBUG)
    {
        for (int i = 0; i < 10; i++)
        {
            if (xargv[i] == NULL)
                break;
            printf("%d:%s\n", i, xargv[i]);
        }
        printf("xargc=%d\n", xargc);
    }
}

int do_cmd(int argc, char *argv[], char *redir)
{
    pid_t pid;
    if ((pid = fork()) < 0)
    {
        fprintf(stderr, "%s", errmsg);
        return pid;
    }
    if (pid == 0)
    { // child
        // Check for extra redirection operator, multiple tokens in redir, and
        // strip leading and trailing whitespace
        if (redir)
        {
            if (DEBUG)
            {
                printf("if (redir)\n");
            }
            int start = 0, end;
            bool in_word = false;
            bool got_word = false;
            for (int i = 0, n = strlen(redir); i < n; i++)
            {
                if (redir[i] == '>')
                {
                    fprintf(stderr, "%s", errmsg);
                    if (DEBUG)
                        printf("extra >\n");
                    exit(1);
                }
                if (got_word && !isspace(redir[i]))
                {
                    fprintf(stderr, "%s", errmsg);
                    if (DEBUG)
                        printf("too many args in redir\n");
                    exit(1);
                }
                if (!in_word && isspace(redir[i]))
                    start++;
                if (!in_word && !isspace(redir[i]))
                    in_word = true;
                if (in_word && isspace(redir[i]))
                {
                    end = i;
                    got_word = true;
                }
            }
            if (!got_word) // redirection with no filename
            {
                fprintf(stderr, "%s", errmsg);
                if (DEBUG)
                    printf("redir with no file\n");
                exit(1);
            }
            redir[end] = '\0';
            redir = redir + start;
            if (DEBUG)
            {
                printf("after parsing, redir=%s\n", redir);
            }

            int fd;
            if ((fd = open(redir, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR)) < 0)
            {
                fprintf(stderr, "%s", errmsg);
                if (DEBUG)
                    printf("open failed\n");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }

        if (execv(argv[0], argv) < 0)
        {
            for (int i = 0; i < pathlen; i++)
            {
                char *s = malloc(strlen(path[i]) + strlen(argv[0]) + 2);
                strcpy(s, path[i]);
                strcat(s, "/");
                strcat(s, argv[0]);
                if (DEBUG)
                    printf("trying %s\n", s);
                execv(s, argv);
            }
            // if we get here, all execs must have failed
            fprintf(stderr, "%s", errmsg);
            if (DEBUG)
                printf("tried all path entries, all execs failed\n");
            exit(1);
        }
        return 0; // never get here; placate compiler
    }
    else
    { // parent
        return pid;
    }
}
