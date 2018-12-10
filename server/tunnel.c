#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <time.h>
#include "tunnel.h"
#include "server.h"
#include "common.h"
#include "listener.h"
#include "../common/command.h"

#define TOKEN_LEN 30

struct entity {
    int fd;
    struct connection *conn;
    struct entity *next;
};

struct hash_map {
    struct entity **arr;
    int size;
};

struct hash_map map;

/**
 * 计算hash值
 * @param mp
 * @param fd
 * @return
 */
int hash(struct hash_map *mp, int fd) {
    return fd % mp->size;
}

/**
 * 初始化Tunnel散列
 */
void initTunnelMap(int size) {
    if (size <= 0) {
        size = 10; //如果size为非法值，将size设为10
    }
    map.arr = malloc(sizeof(struct entity *) * size);
    memset(map.arr, 0, sizeof(struct entity *) * size); //数据初始化--清零
    map.size = size;
}

void put_tunnel(int fd, struct connection *conn) {
    int h = hash(&map, fd);
    struct entity *e = malloc(sizeof(struct entity));
    e->fd = fd;
    e->conn = conn;
    e->next = map.arr[h];
    map.arr[h] = e;
}

struct connection *get_tunnel(int fd) {
    int h = hash(&map, fd);
    struct entity *e = map.arr[h];
    while (e != null) {
        if (e->fd == fd) {
            return e->conn;
        }
        e = e->next;
    }
    return null;
}

boolean remove_tunnel(int fd) {
    int h = hash(&map, fd);
    for (struct entity **curr = &map.arr[h]; *curr;) {
        struct entity *e = *curr;
        if (e->fd == fd) {
            *curr = e->next;
            free(e);
            return true;
        } else
            curr = &e->next;
    }
    return false;
}

char *gen_token() {
    char *str = malloc(TOKEN_LEN + 1);
    srand(time(NULL));//通过时间函数设置随机数种子，使得每次运行结果随机。
    for (int i = 0; i < TOKEN_LEN; i++) {
        int flag = rand() % 3;
        switch (flag) {
            case 0:
                str[i] = rand() % 26 + 'a';
                break;
            case 1:
                str[i] = rand() % 26 + 'A';
                break;
            case 2:
                str[i] = rand() % 10 + '0';
                break;
        }
    }
    str[TOKEN_LEN] = 0;
    return str;
}

int create_tunnel(int epfd, struct connection *conn) {
    int listenfd = create_listener(0, 200, false, true);
    if (listenfd == -1) {
        return -1;
    }

    //获取监听的端口号
    struct sockaddr_in addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    getsockname(listenfd, (struct sockaddr *) &addr, &sin_size);
    //设置连接的相关信息
    struct listen_user *lu = malloc(sizeof(struct listen_user));
    lu->tunnel_conn = conn;
    lu->port = ntohs(addr.sin_port);
    lu->queue = newQueue(sizeof(struct connection *));

    struct connection *listen_user_conn = create_conn(listenfd, S_LISTEN_USER, lu);

    struct tunnel *t = malloc(sizeof(struct tunnel));
    t->listen_user_conn = listen_user_conn;
    t->token = gen_token();

    conn->type = S_TUNNEL; //更改类型为隧道
    conn->ptr = t;

    struct epoll_event ev;
    ev.data.ptr = listen_user_conn;
    ev.events = EPOLLIN | EPOLLET;//边缘触发选项

    // 设置epoll的事件
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
        perror("Set epoll_ctl");
        return -1;
    }

    //将tunnel连接插入散列
    put_tunnel(conn->fd, conn);
    char msg[256];
    sprintf(msg, "%s %d %d %s\n", SUCCESS, conn->fd, lu->port, t->token);
    // 通知客户端隧道创建成功（success 隧道FD 监听用户请求的端口号 Pull请求的Token）
    if (write_data(conn, msg, strlen(msg)) == -1) {
        return -1;
    }

    printf("Change type to tunnel.\n");
    return listenfd;
}