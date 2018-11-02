//
// Created by maqian on 2018/11/1.
//

#include <stdio.h>
#include <memory.h>
#include "common.h"

struct connection *create_conn(SOCKET fd, int type, void *ptr) {
    struct connection *conn = malloc(sizeof(struct connection));
    conn->fd = fd;
    conn->type = type;
    conn->write_buf = null;
    conn->len = 0;
    conn->ptr = ptr;
    return conn;
}

void tag_close_conn(struct connection *conn, Array arr) {
    if (conn == null || conn->tag_close == true) {
        return;
    }

    conn->tag_close = true;
    addArrayForPointer(arr, conn);
    printf("Tag close connection, fd: %d  type: %d\n", conn->fd, conn->type);
    if (conn->type == C_TUNNEL) {
        error("The main socket closed");
        exit(1);
    } else if (conn->type == C_R_SERVER) {
        if (conn->ptr != null) {
            struct r_server_conn *p = conn->ptr;
            tag_close_conn(p->l_server_conn, arr);
        }
    } else if (conn->type == C_L_SERVER) {
        if (conn->ptr != null) {
            struct l_server_conn *p = conn->ptr;
            tag_close_conn(p->r_server_conn, arr);
        }
    }
}

int close_conn(struct connection *conn) {
    printf("Closed connection, fd: %d  type: %d p:%p\n", conn->fd, conn->type, conn);

    int res = closesocket(conn->fd);
    if (conn->ptr) {
        free(conn->ptr);
    }
    if (conn->write_buf && conn->len > 0) {
        free(conn->write_buf);
    }
    free(conn);
    return res;
}

void close_conn_arr(Array arr) {
    for (int i = 0; i < arr->size; i++) {
        close_conn(getArrayForPointer(arr, i));
    }
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