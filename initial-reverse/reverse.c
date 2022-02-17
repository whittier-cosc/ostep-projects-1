#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// This typedef allows us to use `node` as a synonym for `struct node`.
typedef
struct node {
    char *buf;          // a string (one line of text)
    struct node *next;  // pointer to next node in linked list
} node;

int main(int argc, char *argv[]) {

    // If there were too many command-line arguments, print an
    // error message and exit with status 1.
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    // By default, we will read from stdin and write to stdout
    FILE *fin = stdin;
    FILE *fout = stdout;

    // If there were one or two command-line arguments
    // 
    // ($ ./reverse <infile>     or     $ ./reverse <infile> <outfile>),
    // 
    // try to open <infile> for reading and set `fin` to the FILE pointer
    // returned by `fopen`. If `fopen` fails, print the required error message 
    // and exit with status 1.
    if (argc >= 2) {
        if ((fin = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    }

    // If there were two command-line arguments 
    //
    // ($ ./reverse <infile> <outfile>),
    //
    // 1. Call `stat` to obtain a `struct stat` for <infile>. This call should
    // not fail, because if we get here, we have already successfully called
    // `fopen` on <infile>.
    // 
    // 2. Call `stat` to obtain a `struct stat` for <outfile>. If `stat` fails
    // (because it couldn't open the file), print the required error message and
    // exit with status 1. 
    //
    // 3. If <infile> and <outfile> are links to the same file (i.e. the inode
    // numbers in their `struct stat`s are the same), print the required error
    // message and exit with status 1. 
    //
    // 4. Try to open <outfile> for writing and set `fout` to the FILE pointer
    // returned by `fopen`. If `fopen` fails, print the required error message 
    // and exit with status 1.
    if (argc == 3) {
        struct stat st_in, st_out;
        stat(argv[1], &st_in);
        if ((fout = fopen(argv[2], "w")) == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
            exit(1);
        }
        if (stat(argv[2], &st_out) < 0) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
            exit(1);
        }
        if (st_in.st_ino == st_out.st_ino) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
    }

     
    // Read one line at a time from `fin` using `getline`. After
    // reading a line, allocate memory for a new node using `malloc`,
    // store `line` in the node, and insert the node at the beginning
    // of the linked list, then reset `line` to NULL and `linecap` to 0.

    node *head = NULL;  // Pointer to first node in linked list of lines
    char *line = NULL;
    size_t linecap = 0;
    while (getline(&line, &linecap, fin) > 0) {
        node *n = malloc(sizeof(node));
        if (n == NULL) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
        n->buf = line;
        n->next = head;
        head = n;
        line = NULL;
        //printf("%s", n->buf);
    }

    // Use a for-loop to walk through the linked list and print
    // each line to `fout` using `fprintf`.
    for (node *p = head; p != NULL; p = p->next) {
        fprintf(fout, "%s", p->buf);
    }

    return 0;
}

