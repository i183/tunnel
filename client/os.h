#ifndef TUNNEL_OS_H
#define TUNNEL_OS_H

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <signal.h>
#include <memory.h>
#include "../structure/array.h"

#define IGNORE_SIGNAL(sig) signal(sig, SIG_IGN)

/* Handle Header Files */
#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#undef    FD_SETSIZE
#define FD_SETSIZE          (1024)
#include <ws2tcpip.h>
#include <unistd.h>

#undef    errno
#define   errno              WSAGetLastError()

#define IGNORE_SIGPIPE()

// connect链接还在进行中, linux显示 EINPROGRESS，winds是 WSAEWOULDBLOCK
#define ECONNECTED           WSAEWOULDBLOCK

typedef int socklen_t;
typedef SOCKET socket_t;

#else

#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/select.h>
#include <sys/resource.h>

#define INVALID_SOCKET      (~0)
#define SOCKET_ERROR        (-1)

#define IGNORE_SIGPIPE()    IGNORE_SIGNAL(SIGPIPE)

#define ECONNECTED          EINPROGRESS

typedef int socket_t;

#endif

typedef struct sockaddr_in sockaddr_t;

void socket_start();

socket_t socket_stream();

sockaddr_t create_sockaddr(char *ip, int port);

int socket_close(socket_t s);

int socket_set_block(socket_t s);

int socket_set_nonblock(socket_t s);

int socket_connect(socket_t s, const sockaddr_t *addr);

int socket_addr(const char *ip, uint16_t port, sockaddr_t *addr);

typedef struct {
    Array li;
    int num; //可写、可读或异常数量
} fd_list;

/**
 * select io
 * @param rl 监测可写列表
 * @param wl 监测可读列表
 * @param el 监测异常列表
 * @param timeout_ms 超时时间（毫秒）-1 无限等待  0立即返回
 * @return
 */
int select_os(fd_list *rl, fd_list *wl, fd_list *el, int timeout_ms);

#endif //TUNNEL_OS_H
