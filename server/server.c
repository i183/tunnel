#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "../common/global.h"

#define MAX_EVENT 20
#define READ_BUF_LEN 1024

const char *pw = null;

struct connection *create_conn(int fd, int type, void *ptr) {
    struct connection *conn = malloc(sizeof(struct connection));
    conn->fd = fd;
    conn->type = type;
    conn->ptr = null;
    return conn;
}

int close_conn(struct connection *conn) {
    printf("Closed connection, fd: %d  type: %d\n", conn->fd, conn->type);
    int res = close(conn->fd);
    if (conn->ptr) {
        free(conn->ptr); // TODO 删除指针数据
    }
    free(conn);
    return res;
}

int start(int port, char *password) {
    pw = password;

    int epfd = 0;
    int listenfd = 0;

    struct epoll_event ev, event[MAX_EVENT];

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listenfd) {
        perror("Open listen socket");
        return -1;
    }

    int on = 1;
    // 打开 socket 端口复用, 防止测试的时候出现 Address already in use
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
        perror("Set socket");
        return -1;
    }

    //绑定端口
    if (bind(listenfd, (const struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Bind port");
        return -1;
    }

    if (make_socket_non_blocking(listenfd) == -1) {
        perror("Make socket non blocking");
        return -1;
    }

    if (listen(listenfd, 200) == -1) {
        perror("Start listen");
        return -1;
    }

    // 创建epoll实例
    epfd = epoll_create1(0);
    if (epfd == 1) {
        perror("Create epoll instance");
        return -1;
    }

    struct connection *conn = create_conn(listenfd, S_LISTEN_CLIENT, NULL);

    ev.data.ptr = conn;
    ev.events = EPOLLIN | EPOLLET;//边缘触发选项
    // 设置epoll的事件
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
        perror("Set epoll_ctl");
        return -1;
    }

    while (true) {
        // 等待事件
        int wait_count = epoll_wait(epfd, event, MAX_EVENT, -1);
        printf("wait_count: %d\n", wait_count);

        for (int i = 0; i < wait_count; i++) {
            struct epoll_event e = event[i];
            struct connection *conn = (struct connection *) e.data.ptr;
            uint32_t events = event[i].events;

            // 判断epoll是否发生错误
            if (events & EPOLLERR || events & EPOLLHUP) {
                printf("Epoll has error\n");
                close_conn(conn);
                continue;
            }

            if (conn->type == 1) {
                handler_1(epfd, &e);
            } else if (conn->type == 2) {
                handler_2(epfd, &e);
            } else if (conn->type == 3) {
                handler_3(epfd, &e);
            } else if (conn->type == 4) {
                handler_4(epfd, &e);
            } else if (conn->type == 5) {
                handler_5(epfd, &e);
            } else if (conn->type == 6) {
                handler_6(epfd, &e);
            }

        }
    }

    return 0;
}

/**
 * 处理"监听客户端请求的Socket"事件
 * @return
 */
int handler_1(int epfd, const struct epoll_event *e) {
    struct connection *conn = (struct connection *) e->data.ptr;
    while (true) {
        struct sockaddr_in in_addr = {0};
        socklen_t in_addr_len = sizeof(in_addr);
        int accp_fd = accept(conn->fd, (struct sockaddr *) &in_addr, &in_addr_len);
        if (accp_fd == -1) {
            break;
        }

        printf("Accept client IP:%s, Port:%d\n", inet_ntoa(in_addr.sin_addr), ntohs(in_addr.sin_port));

        if (make_socket_non_blocking(accp_fd) == -1) {
            perror("Accept make socket non blocking");
            return -1;
        }

        struct connection *conn = create_conn(accp_fd, S_UNKNOWN, null);
        struct epoll_event ev;
        ev.data.ptr = conn;
        ev.events = EPOLLIN | EPOLLET;//边缘触发选项

        // 为新accept的 file describe 设置epoll事件
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, accp_fd, &ev) == -1) {
            perror("epoll_ctl");
            return -1;
        }

    }
    return 0;
}

/**
 * 处理"监听用户请求的Socket"事件
 * @return
 */
int handler_2(int epfd, const struct epoll_event *e) {
    return 0;
}

/**
 * 处理"未知"事件
 * @return
 */
int handler_3(int epfd, const struct epoll_event *e) {
    struct connection *conn = (struct connection *) e->data.ptr;
    boolean done = false;
    while (true) {
        char buf[READ_BUF_LEN];
        ssize_t len = read(conn->fd, buf, READ_BUF_LEN);
        printf("handler_3 len: %ld\n", len);
        if (len == -1) {
            if (EAGAIN != errno) {
                perror("Read data");
                done = true;
            }
            break;
        } else if (!len) {
            done = true;
            break;
        }
        buf[len] = 0;

        printf("handler_3 Read the content: %s\n", buf);

        char *command = strtok(buf, " ");
        if (command == null) {
            done = true;
            break;
        } else if (strcmp(command, "tunnel") == 0) {
            char *password = strtok(null, " ");
            if (strcmp(password, pw)) {
                //密码错误
                char *str = "密码错误";
                write(conn->fd, str, strlen(str));
                done = true;
                break;
            }

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
                done = true;
                break;
            }

            printf("Change type to tunnel.\n");
            break;
        } else if (strcmp(command, "pull") == 0) {
            printf("pull\n");
            break;
        } else {
            done = true;
            break;
        }
    }

    if (done) {
        close_conn(conn);
    }
    return 0;
}

/**
 * 处理"与客户端建立的隧道"事件
 * @return
 */
int handler_4(int epfd, const struct epoll_event *e) {
    return 0;
}

/**
 * 处理"客户端处理请求连接"事件
 * @return
 */
int handler_5(int epfd, const struct epoll_event *e) {
    return 0;
}

/**
 * 处理"用户的请求连接"事件
 * @return
 */
int handler_6(int epfd, const struct epoll_event *e) {
    return 0;
}