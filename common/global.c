#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <memory.h>

void error(char *s) {
    fprintf(stderr, "Error: %s\n", s);
    exit(EXIT_FAILURE);
}


int make_socket_non_blocking(int fd) {
    int flags, s;
    // 获取当前flag
    flags = fcntl(fd, F_GETFL, 0);
    if (-1 == flags) {
        perror("Get fd status");
        return -1;
    }

    flags |= O_NONBLOCK;

    // 设置flag
    s = fcntl(fd, F_SETFL, flags);
    if (-1 == s) {
        perror("Set fd status");
        return -1;
    }
    return 0;
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