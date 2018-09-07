#include <stdio.h>
#include <stdlib.h>

void error(char *s) {
    fprintf(stderr, "Error: %s\n", s);
    exit(EXIT_FAILURE);
}