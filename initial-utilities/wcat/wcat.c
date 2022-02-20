#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *f;
    char buf[80];
    for (int i = 1; i < argc; i++) {
        if ((f = fopen(argv[i], "r")) == NULL) {
            printf("wcat: cannot open file\n");
            exit(1);
        }
        while (fgets(buf, 80, f) != NULL) {
            printf("%s", buf);
        }
    }

    return 0;
}