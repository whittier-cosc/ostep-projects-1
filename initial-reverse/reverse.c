#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef
struct node {
    char *buf;
    struct node *next;
} node;

int main(int argc, char *argv[]) {
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }
    FILE *fin = stdin, *fout = stdout;
    if (argc >= 2) {
        if ((fin = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    }
    if (argc == 3) {
        struct stat st_in, st_out;
        if (stat(argv[1], &st_in) < 0) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
        if (stat(argv[2], &st_out) == 0 && st_in.st_ino == st_out.st_ino) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
        if ((fout = fopen(argv[2], "w")) == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
            exit(1);
        }
    }

    node *head = NULL;
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

    for (node *p = head; p != NULL; p = p->next) {
        fprintf(fout, "%s", p->buf);
    }

    return 0;
}

