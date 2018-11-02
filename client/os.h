#ifndef TUNNEL_OS_H
#define TUNNEL_OS_H

/* Handle Header Files */
#if defined _WIN32
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include "../structure/array.h"

#endif

/* Handle Minor Differences */
#if defined _WIN32
#define SOCKERR() WSAGetLastError()
#define close(s) closesocket(s)
#define errno SOCKERR()
#else
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

typedef struct {
    Array li;
    int num; //可写、可读或异常数量
} fd_list;

int sock_init();

int sock_quit();

int closesocket(SOCKET sock);

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
