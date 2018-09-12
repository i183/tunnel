#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include "tunnel.h"
#include "server.h"
#include "common.h"
#include "listener.h"

int create_tunnel(int epfd, struct connection *conn) {
    struct tunnel *t = malloc(sizeof(struct tunnel));
    t->token = "0101"; // TODO 生成Token

    conn->type = S_TUNNEL; //更改类型为隧道
    conn->ptr = t;

    struct epoll_event ev;
    ev.data.ptr = conn;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET; //边缘触发选项

    // 更改epoll事件 增加 EPOLLOUT 事件
    if (epoll_ctl(epfd, EPOLL_CTL_MOD, conn->fd, &ev) == -1) {
        perror("epoll_ctl");
        return -1;
    }

    int listenfd = create_listener(0, 200, false, true);
    if (listenfd == -1) {
        return -1;
    }

    struct connection *listen_user_conn = create_conn(listenfd, S_LISTEN_USER, null);

    ev.data.ptr = listen_user_conn;
    ev.events = EPOLLIN | EPOLLET;//边缘触发选项

    // 设置epoll的事件
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
        perror("Set epoll_ctl");
        return -1;
    }

    struct sockaddr_in addr;
    getsockname(listen_user_conn->fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));

    char msg[256];
    msg[0] = 0;
    strcat(msg, "success ");

    printf("Change type to tunnel.\n");
    return listenfd;
}