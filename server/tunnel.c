//
// Created by maqian on 2018/9/11.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include "tunnel.h"
#include "server.h"

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

    printf("Change type to tunnel.\n");
    return 0;
}