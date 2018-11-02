#include <stdio.h>
#include "client.h"
#include "common.h"

int create_tunnel(char *ip, int r_port, int l_port, char *password) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        perror("invalid socket");
        return -1;
    }

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(r_port);
    dest_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (struct sockaddr *) &dest_addr, sizeof(dest_addr)) == SOCKET_ERROR) {
        perror("connect error");
        closesocket(sock);
        return -1;
    }

    struct connection *tunnel = create_conn

    write(conn->fd, buf, len);
    return 0;
}