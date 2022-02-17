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
        // TODO
    }

    // If there were two command-line arguments 
    //
    // ($ ./reverse <infile> <outfile>),
    //
    // 1. Call `stat` to obtain a `struct stat` for <infile>. This call should
    // not fail, because if we get here, we have already successfully called
    // `fopen` on <infile>.
    // 
    // 2. Try to open <outfile> for writing and set `fout` to the FILE pointer
    // returned by `fopen`. If `fopen` fails, print the required error message 
    // and exit with status 1.
    //
    // 3. Call `stat` to obtain a `struct stat` for <outfile>. If `stat` fails
    // (because it couldn't open the file), print the required error message and
    // exit with status 1. 
    //
    // 4. If <infile> and <outfile> are links to the same file (i.e. the inode
    // numbers in their `struct stat`s are the same), print the required error
    // message and exit with status 1. 
    //
    if (argc == 3) {
        struct stat st_in, st_out;
        // TODO
    }


    // Now we read lines from <infile> and print them in reverse order to
    // <outfile>.   

    node *head = NULL;  // pointer to first node in linked list
    char *line = NULL;
    size_t linecap = 0;
    // Using a while-loop, read one line at a time from `fin` using `getline`.
    // After reading a line, allocate memory for a new node using `malloc`,
    // store `line` in the node, and insert the node at the beginning of the
    // linked list, then reset `line` to NULL and `linecap` to 0.

    // TODO
    


    // Finally, use a for-loop to walk through the linked list and print each
    // line to `fout` using `fprintf`.
    
    // TODO

    return 0;
}
