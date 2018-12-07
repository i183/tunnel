//
// Created by maqian on 2018/11/1.
//

#include <stdio.h>
#include <memory.h>
#include "common.h"
#include "os.h"

struct connection *create_conn(socket_t fd, int type, boolean is_asyn, void *ptr) {
    struct connection *conn = malloc(sizeof(struct connection));
    conn->fd = fd;
    conn->type = type;
    conn->tag_close = false;
    conn->write_buf = null;
    conn->len = 0;
    conn->is_asyn = is_asyn;
    conn->asyn_conn = false;
    conn->ptr = ptr;

    if (is_asyn) {
        socket_set_nonblock(conn->fd);
    }
    return conn;
}

void verify_asyn_conn(fd_list *fl, Array tag) {
    char optval;
    socklen_t optlen = sizeof(optval);
    for (int i = 0; i < fl->num; ++i) {
        struct connection *conn = getArrayForPointer(fl->li, i);
        if (!conn->tag_close && conn->is_asyn && !conn->asyn_conn) {
            if (getsockopt(conn->fd, SOL_SOCKET, SO_ERROR, &optval, &optlen) && optval) {
                //一步连接失败，关闭连接
                tag_close_conn(conn, tag);
            } else {
                conn->asyn_conn = true;
                handler_write(conn);
            }
        }
    }
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

    int res = socket_close(conn->fd);
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

int wait_data(struct connection *conn, const void *buf, size_t len) {
    if (conn->len > 0) {
        void *bs = malloc(conn->len + len);
        memcpy(bs, conn->write_buf, conn->len);
        memcpy(bs + conn->len, buf, len);
        free(conn->write_buf);
        conn->write_buf = bs;
        conn->len = conn->len + len;
        return 1;
    } else {
        conn->write_buf = malloc(len);
        memcpy(conn->write_buf, buf, len);
        conn->len = len;
        return 0;
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

/**
 * 处理写入事件
 * @param e
 * @return
 */
int handler_write(struct connection *conn) {
    // 检测当前连接是否可写入
    if (conn->is_asyn && !conn->asyn_conn) {
        return -2;
    }

    //检测是否有待写数据
    if (conn->len <= 0) {
        return 0;
    }

    ssize_t len = write(conn->fd, conn->write_buf, conn->len);
    if ((len == -1 && EAGAIN != errno) || len == 0) {
        //写入数据时出错或连接关闭
        perror("write");
        return -1;
    } else if (len == -1) {
        printf("len == -1\n");
        return 1;
    } else if (len > 0) {
        //写入数据成功
        //删除待写数据
        free(conn->write_buf);
        conn->len = 0;
    }
    return 0;
}
