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
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#undef    FD_SETSIZE
#define FD_SETSIZE          (1024)
#include <ws2tcpip.h>

#undef    errno
#define   errno              WSAGetLastError()

#define IGNORE_SIGPIPE()

// connect链接还在进行中, linux显示 EINPROGRESS，winds是 WSAEWOULDBLOCK
#define ECONNECTED           WSAEWOULDBLOCK

typedef int socklen_t;
typedef SOCKET socket_t;

void _socket_start(void) {
    WSACleanup();
}
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

void socket_start() {
#ifdef _MSC_VER
    WSADATA wsad;
    WSAStartup(WINSOCK_VERSION, &wsad);
    atexit(_socket_start);
#endif
    IGNORE_SIGPIPE();
}

socket_t socket_stream() {
    return socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
}

sockaddr_t create_sockaddr(char *ip, int port) {
#ifdef _MSC_VER
    // Windows create sockaddr
#else
    sockaddr_t addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    return addr;
#endif
}

int socket_close(socket_t s) {
#ifdef _MSC_VER
    return closesocket(s);
#else
    return close(s);
#endif
}

int socket_set_block(socket_t s) {
#ifdef _MSC_VER
    u_long mode = 0;
    return ioctlsocket(s, FIONBIO, &mode);
#else
    int mode = fcntl(s, F_GETFL, 0);
    if (mode == SOCKET_ERROR)
        return SOCKET_ERROR;
    if (mode & O_NONBLOCK)
        return fcntl(s, F_SETFL, mode & ~O_NONBLOCK);
    return 0;
#endif
}

int socket_set_nonblock(socket_t s) {
#ifdef _MSC_VER
    u_long mode = 1;
    return ioctlsocket(s, FIONBIO, &mode);
#else
    int mode = fcntl(s, F_GETFL, 0);
    if (mode == SOCKET_ERROR)
        return SOCKET_ERROR;
    if (mode & O_NONBLOCK)
        return 0;
    return fcntl(s, F_SETFL, mode | O_NONBLOCK);
#endif
}

int socket_connect(socket_t s, const sockaddr_t *addr) {
    return connect(s, (const struct sockaddr *) addr, sizeof(*addr));
}

int socket_addr(const char *ip, uint16_t port, sockaddr_t *addr) {
    if (!ip || !*ip || !addr) {
        fprintf(stderr, "check empty ip = %s, port = %hu, addr = %p.\n", ip, port, addr);
        return -1;
    }

    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = inet_addr(ip);
    if (addr->sin_addr.s_addr == INADDR_NONE) {
        struct hostent *host = gethostbyname(ip);
        if (!host || !host->h_addr) {
            fprintf(stderr, "check ip is error = %s.\n", ip);
            return -1;
        }
        // 尝试一种, 默认ipv4
        memcpy(&addr->sin_addr, host->h_addr, host->h_length);
    }
    memset(addr->sin_zero, 0, sizeof addr->sin_zero);

    return 0;
}

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
