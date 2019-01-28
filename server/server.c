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
#include "tunnel.h"
#include "common.h"
#include "listener.h"
#include "../common/command.h"
#include "cmd.h"

#define MAX_EVENT 20
#define READ_BUF_LEN 1024
#define TUNNEL_MAP_SIZE 20

const char *pw = null;

Array tag;

int handler_write(const struct epoll_event *e);

int handler_1(int epfd, const struct epoll_event *e);

int handler_2(int epfd, const struct epoll_event *e);

int handler_3(int epfd, const struct epoll_event *e);

int handler_4(int epfd, const struct epoll_event *e);

int handler_5(int epfd, const struct epoll_event *e);

int handler_6(int epfd, const struct epoll_event *e);


int start(int port, char *password) {
    pw = password;
    tag = newArrayDefault(sizeof(struct connection *));
    initTunnelMap(TUNNEL_MAP_SIZE);

    int epfd = 0;
    int listenfd = create_listener(port, 200, true, true);
    //验证listener是否创建成功
    if (listenfd == -1) {
        return -1;
    }
    struct connection *conn = create_conn(listenfd, S_LISTEN_CLIENT, NULL);

    struct epoll_event ev, event[MAX_EVENT];
    log_info("Listening...\n");
    // 创建epoll实例
    epfd = epoll_create1(0);
    if (epfd == 1) {
        log_err("Create epoll instance");
        return -1;
    }

    ev.data.ptr = conn;
    ev.events = EPOLLIN | EPOLLET;//边缘触发选项
    // 设置epoll的事件
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
        log_err("Set epoll_ctl");
        return -1;
    }

    while (true) {
        // 等待事件
        int wait_count = epoll_wait(epfd, event, MAX_EVENT, -1);

        for (int i = 0; i < wait_count; i++) {
            struct epoll_event e = event[i];
            struct connection *conn = (struct connection *) e.data.ptr;
            uint32_t events = event[i].events;

            // 判断epoll是否发生错误
            if (events & EPOLLERR || events & EPOLLHUP) {
                log_err("Epoll has error\n");
                tag_close_conn(conn, tag);
                continue;
            }

            //可写事件
            if (events & EPOLLOUT && conn->tag_close == false) {
                if (handler_write(&e) == -1) {
                    //写入数据出错，关闭连接
                    tag_close_conn(conn, tag);
                    continue;
                }
            }

            if (conn->tag_close == true) {
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
        close_conn_arr(tag);
        resetArrayEmpty(tag);
    }

    return 0;
}

/**
 * 处理写入事件
 * @param e
 * @return
 */
int handler_write(const struct epoll_event *e) {
    struct connection *conn = (struct connection *) e->data.ptr;
    //检测是否有待写数据
    if (conn->len <= 0) {
        return 0;
    }

    ssize_t len = write(conn->fd, conn->write_buf, conn->len);
    if ((len == -1 && EAGAIN != errno) || len == 0) {
        //写入数据时出错或连接关闭
        log_err("write");
        return -1;
    } else if (len == -1) {
        return 1;
    } else if (len > 0) {
        //写入数据成功
        //删除待写数据
        free(conn->write_buf);
        conn->len = 0;
    }
    return 0;
}

int read_write_client(struct connection *conn) {
    if (conn->type != S_CLIENT) {
        return -1;
    }

    struct connection *uc = ((struct client_conn *) conn->ptr)->user_conn;

    while (true) {
        if (uc->len > 0) {
            //存在待写入数据，结束循环
            break;
        }

        char buf[READ_BUF_LEN];
        ssize_t len = read(conn->fd, buf, READ_BUF_LEN);
        if (len == -1) {
            if (EAGAIN != errno) {
                log_err("Read data");
                return -1;
            }
            break;
        } else if (len == 0) {
            return -1;
        }

        int flag = write_data(uc, buf, len);
        if (flag == -1) {
            return -1;
        }
    }
    return 0;
}


int read_write_user(struct connection *conn) {
    if (conn->type != S_USER) {
        return -1;
    }

    struct connection *cc = ((struct user_conn *) conn->ptr)->client_conn;

    while (true) {
        if (cc->len > 0) {
            //存在待写入数据，结束循环
            break;
        }

        char buf[READ_BUF_LEN + 1];
        ssize_t len = read(conn->fd, buf, READ_BUF_LEN);
        if (len == -1) {
            if (EAGAIN != errno) {
                log_err("Read data");
                return -1;
            }
            break;
        } else if (len == 0) {
            return -1;
        }

        int flag = write_data(cc, buf, len);
        if (flag == -1) {
            return -1;
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

        //printf("Accept client IP:%s, Port:%d\n", inet_ntoa(in_addr.sin_addr), ntohs(in_addr.sin_port));

        if (make_socket_non_blocking(accp_fd) == -1) {
            log_err("Accept make socket non blocking");
            return -1;
        }

        struct connection *uc = create_conn(accp_fd, S_UNKNOWN, null);
        struct epoll_event ev;
        ev.data.ptr = uc;
        ev.events = EPOLLIN | EPOLLOUT | EPOLLET;//边缘触发选项

        // 为新accept的 file describe 设置epoll事件
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, accp_fd, &ev) == -1) {
            log_err("epoll_ctl");
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
    struct connection *conn = (struct connection *) e->data.ptr;
    while (true) {
        struct sockaddr_in in_addr = {0};
        socklen_t in_addr_len = sizeof(in_addr);
        int accp_fd = accept(conn->fd, (struct sockaddr *) &in_addr, &in_addr_len);
        if (accp_fd == -1) {
            break;
        }

        //printf("Accept user IP:%s, Port:%d\n", inet_ntoa(in_addr.sin_addr), ntohs(in_addr.sin_port));

        if (make_socket_non_blocking(accp_fd) == -1) {
            log_err("Accept make socket non blocking");
            return -1;
        }

        struct connection *user_conn = create_conn(accp_fd, S_USER, null);
        struct listen_user *lu = conn->ptr;
        inQueueForPointer(lu->queue, user_conn);

        char msg[20];
        sprintf(msg, "%s\n", REQUEST);
        //printf("Send request\n");
        int flag = write_data(lu->tunnel_conn, msg, strlen(msg)); //通知客户端有新的请求
        if (flag == -1) {
            tag_close_conn(conn, tag);
            break;
        }

    }
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
        if (conn->len > 0) {
            //存在待写入数据，结束循环
            break;
        }

        char buf[READ_BUF_LEN];
        ssize_t len = read(conn->fd, buf, READ_BUF_LEN);
        if (len == -1) {
            if (EAGAIN != errno) {
                log_err("Read data");
                done = true;
            }
            break;
        } else if (len == 0) {
            done = true;
            break;
        }
        buf[len] = 0;

        line_to_zero(buf);
        //printf("handler_3 Read the content: %s\n", buf);

        char command[30] = "";
        sscanf(buf, "%s ", command);
        if (command == null) {
            done = true;
            break;
        } else if (strcmp(command, TUNNEL) == 0) {
            //处理tunnel命令
            if (tunnel_cmd(epfd, conn, buf, pw) == -1) {
                done = true;
                break;
            }
            break;
        } else if (strcmp(command, PULL) == 0) {
            //处理pull命令
            //printf("Receive pull\n");
            if (pull_cmd(epfd, conn, buf) == -1) {
                done = true;
                break;
            }
            break;
        } else {
            done = true;
            break;
        }
    }

    if (done) {
        tag_close_conn(conn, tag);
    }
    return 0;
}

/**
 * 处理"与客户端建立的隧道"事件
 * @return
 */
int handler_4(int epfd, const struct epoll_event *e) {
    struct connection *conn = (struct connection *) e->data.ptr;
    boolean done = false;
    while (true) {
        if (conn->len > 0) {
            //存在待写入数据，结束循环
            break;
        }

        char buf[READ_BUF_LEN];
        ssize_t len = read(conn->fd, buf, READ_BUF_LEN);
        if (len == -1) {
            if (EAGAIN != errno) {
                log_err("Read data");
                done = true;
            }
            break;
        } else if (len == 0) {
            done = true;
            break;
        }
        buf[len] = 0;
        //printf("handler_4 Read the content: %s\n", buf);
    }

    if (done) {
        tag_close_conn(conn, tag);
    }
    return 0;
}

/**
 * 处理"客户端处理请求连接"事件
 * @return
 */
int handler_5(int epfd, const struct epoll_event *e) {
    struct connection *conn = (struct connection *) e->data.ptr;
    struct connection *uc = ((struct client_conn *) conn->ptr)->user_conn;

    if (e->events & EPOLLIN) {
        if (read_write_client(conn) == -1) {
            tag_close_conn(conn, tag);
            return -1;
        }
    }

    if (e->events & EPOLLOUT) {
        if (read_write_user(uc) == -1) {
            tag_close_conn(conn, tag);
            return -1;
        }
    }

    return 0;
}

/**
 * 处理"用户的请求连接"事件
 * @return
 */
int handler_6(int epfd, const struct epoll_event *e) {
    struct connection *conn = (struct connection *) e->data.ptr;
    struct connection *cc = ((struct user_conn *) conn->ptr)->client_conn;

    if (e->events & EPOLLIN) {
        if (read_write_user(conn) == -1) {
            tag_close_conn(conn, tag);
            return -1;
        }
    }

    if (e->events & EPOLLOUT) {
        if (read_write_client(cc) == -1) {
            tag_close_conn(conn, tag);
            return -1;
        }
    }
    return 0;
}