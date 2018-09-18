#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <errno.h>
#include <memory.h>
#include "common.h"
#include "server.h"
#include "tunnel.h"

struct connection *create_conn(int fd, int type, void *ptr) {
    struct connection *conn = malloc(sizeof(struct connection));
    conn->fd = fd;
    conn->type = type;
    conn->write_buf = null;
    conn->len = 0;
    conn->ptr = ptr;
    return conn;
}

int close_conn(struct connection *conn) {
    printf("Closed connection, fd: %d  type: %d\n", conn->fd, conn->type);
    if (conn->type == S_TUNNEL) {
        remove_tunnel(conn->fd);
    } else if (conn->type == S_LISTEN_USER) {
        struct listen_user *lu = conn->ptr;
        while (!isQueueEmpty(lu->queue)) {
            close_conn(outQueueForPointer(lu->queue));
        }
        freeQueue(lu->queue);
    }

    int res = close(conn->fd);
    if (conn->ptr) {
        free(conn->ptr);
    }
    if (conn->write_buf) {
        free(conn->write_buf);
    }
    free(conn);
    return res;
}

int write_data(struct connection *conn, const void *buf, size_t len) {
    //检测是否有待写数据
    if (conn->len > 0) {
        void *bs = malloc(conn->len + len);
        memcpy(bs, conn->write_buf, conn->len);
        memcpy(bs + conn->len, buf, len);
        free(conn->write_buf);
        conn->write_buf = bs;
        conn->len = conn->len + len;
        return 1;
    }
    ssize_t res = write(conn->fd, buf, len);
    if ((res == -1 && EAGAIN != errno) || res == 0) {
        //写入数据时出错或连接关闭
        perror("write");
        return -1;
    } else if (res == -1) {
        conn->write_buf = malloc(len);
        memcpy(conn->write_buf, buf, len);
        conn->len = len;
        return 1;
    }
    return 0;
}