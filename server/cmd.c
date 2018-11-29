#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "cmd.h"
#include "../common/command.h"
#include "common.h"
#include "tunnel.h"

int tunnel_cmd(int epfd, struct connection *conn, const char *cmd, const char *pw) {
    char password[50] = "";
    sscanf(cmd, "tunnel %s", password);
    printf("cmd: %s, pw: %s\n", cmd, password);
    if (strcmp(password, pw)) {
        //密码错误
        char msg[256];
        sprintf(msg, "%s %s\n", C_ERROR, "密码错误");
        write_data(conn, msg, strlen(msg));
        return -1;
    }

    if (create_tunnel(epfd, conn) == -1) {
        return -1;
    }
    return 0;
}

int pull_cmd(int epfd, struct connection *conn, const char *cmd) {
    printf("pull\n");
    int fd = -1;
    char token[50] = "";
    sscanf(cmd, "pull %d %s", &fd, token);
    if (fd == -1 || strlen(token) == 0) {
        char msg[256];
        sprintf(msg, "%s %s\n", C_ERROR, "参数错误");
        write_data(conn, msg, strlen(msg));
        return -1;
    }

    struct tunnel *tp = get_tunnel(fd)->ptr;
    if (strcmp(token, tp->token) != 0) {
        char msg[256];
        sprintf(msg, "%s %s\n", C_ERROR, "Token错误");
        write_data(conn, msg, strlen(msg));
        return -1;
    }

    struct listen_user *lu = tp->listen_user_conn->ptr;
    if (isQueueEmpty(lu->queue)) {
        char msg[256];
        sprintf(msg, "%s %s\n", C_ERROR, "没有待处理的连接");
        write_data(conn, msg, strlen(msg));
        return -1;
    }
    
    struct client_conn *cc = malloc(sizeof(struct client_conn));
    cc->user_conn = outQueueForPointer(lu->queue);
    conn->ptr = cc;
    conn->type = S_CLIENT;

    struct user_conn *uc = malloc(sizeof(struct user_conn));
    uc->client_conn = conn;
    cc->user_conn->ptr = uc;

    struct epoll_event ev;
    ev.data.ptr = cc->user_conn;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;//边缘触发选项

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, cc->user_conn->fd, &ev) == -1) {
        perror("epoll_ctl");
        return -1;
    }

    return 0;
}