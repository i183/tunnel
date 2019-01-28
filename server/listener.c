#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include "listener.h"
#include "common.h"

int create_listener(int port, int backlog, boolean reuse_addr, boolean non_blocking) {
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        log_err("Open listen socket");
        return -1;
    }

    int on = 1;
    // 打开 socket 端口复用, 防止测试的时候出现 Address already in use
    if (reuse_addr && setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
        log_err("Set socket");
        return -1;
    }

    if (bind(listenfd, (const struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        log_err("Bind port");
        return -1;
    }

    if (non_blocking && make_socket_non_blocking(listenfd) == -1) {
        log_err("Make socket non blocking");
        return -1;
    }

    if (listen(listenfd, backlog) == -1) {
        log_err("Start listen");
        return -1;
    }

    return listenfd;
}