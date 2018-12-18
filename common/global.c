#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <memory.h>

#include "global.h"

void error(char *s) {
    fprintf(stderr, "Error: %s\n", s);
    exit(EXIT_FAILURE);
}

void line_to_zero(char *s) {
    char *index = strchr(s, '\n');
    if (index) {
        s[index - s] = 0;
    }
}

/**
 * 获取系统类型
 * @return 1.linux 2.os x 3.windows 4.unix 5.其他
 */
int get_system_type() {
#ifdef __linux__
    return 1;
#elif __APPLE__
    return 2;
#elif defined(_WIN32) || defined(WIN32)
    return 3;
#elif __unix_
    return 4;
#else
    return 5;
#endif
}

boolean memeq(void *p1, void *p2, int n) {
    char *c1 = p1, *c2 = p2;
    for (; n > 0; c1++, c2++, n--) {
        if (*c1 != *c2) {
            return false;
        }
    }
    return true;
}

int enter(void *dest, size_t dest_size, char *def) {
    if (fgets(dest, dest_size, stdin) != NULL) {
        /**
         * Find the newline and, if present, zero it out
         */
        char *newline = strchr(dest, '\n');
        if (newline)
            *newline = 0;

        if (strlen(dest) == 0) {
            strcpy(dest, def);
            return 0;
        } else {
            return 1;
        }
    }
    return -1;
}

void clear_stdin() {
    char c;
    while ((c = getchar()) != EOF && c != '\n') {
        printf("%c", c);
    };
}