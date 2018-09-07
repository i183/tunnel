#include <stdio.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <zconf.h>
#include <errno.h>
#include <fcntl.h>


int make_socket_non_blocking (int fd) {
    int flags, s;
    // 获取当前flag
    flags = fcntl(fd, F_GETFL, 0);
    if (-1 == flags) {
        perror("Get fd status");
        return -1;
    }

    flags |= O_NONBLOCK;

    // 设置flag
    s = fcntl(fd, F_SETFL, flags);
    if (-1 == s) {
        perror("Set fd status");
        return -1;
    }
    return 0;
}
int main() {
    int server_sockfd;//服务器端套接字
    int client_sockfd;//客户端套接字
    int len;
    struct sockaddr_in my_addr;   //服务器网络地址结构体
    struct sockaddr_in remote_addr; //客户端网络地址结构体
    socklen_t sin_size = sizeof(struct sockaddr_in);
    int enable = 1;
    char buf[BUFSIZ];  //数据传送的缓冲区
    memset(&my_addr, 0, sizeof(my_addr)); //数据初始化--清零
    my_addr.sin_family = AF_INET; //设置为IP通信
    my_addr.sin_addr.s_addr = INADDR_ANY;//服务器IP地址--允许连接到所有本地地址上
    my_addr.sin_port = htons(8877); //服务器端口号

    /*创建服务器端套接字--IPv4协议，面向连接通信，TCP协议*/
    if ((server_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 1;
    }

    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt");
        return 1;
    }

    /*将套接字绑定到服务器的网络地址上*/
    if (bind(server_sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) < 0) {
        perror("bind");
        return 1;
    }

    /*监听连接请求--监听队列长度为5*/
    listen(server_sockfd, 0);
    getsockname(server_sockfd, (struct sockaddr *) &my_addr, &sin_size);
    printf("IP: %s\n", inet_ntoa(my_addr.sin_addr));
    printf("Port: %d\n", ntohs(my_addr.sin_port));


    /*等待客户端连接请求到达*/
    if ((client_sockfd = accept(server_sockfd, (struct sockaddr *) &remote_addr, &sin_size)) < 0) {
        perror("accept");
        return 1;
    }

    //getpeername(client_sockfd, (struct sockaddr *) &remote_addr, &sin_size);

    printf("accept client %s\n", inet_ntoa(remote_addr.sin_addr));
    printf("accept client port %d\n", ntohs(remote_addr.sin_port));
    recv(client_sockfd, buf, BUFSIZ, 0);
    int fd;
    if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("fd err:");
        return 1;
    }

    printf("make non-blocking: %d\n", make_socket_non_blocking(fd));
    if (connect(fd, (struct sockaddr *) &remote_addr, sizeof(struct sockaddr)) < 0) {
        perror("connect");
        //return 1;
    }

    usleep(1000);

    len = send(fd, &fd, 1025, 0);//发送欢迎信息
    while (1) {
        if (send(fd, "ok\n", 1024, 0) == -1) {
            ssize_t i = send(fd, &fd, 1, 0);
            if (i == -1) {
                printf("send len:%d\n", i);
            }
        }
    }
    //printf("send len:%d\n", send(fd, &fd, 10, 0));
    perror("send:");

    /*接收客户端的数据并将其发送给客户端--recv返回接收到的字节数，send返回发送的字节数*/
    while ((len = recv(fd, buf, BUFSIZ, 0)) > 0) {
        buf[len] = '\0';
        printf("%s\n", buf);
        if (send(fd, buf, len, 0) < 0) {
            perror("write");
            return 1;
        }
    }
    close(fd);
    close(client_sockfd);
    close(server_sockfd);
    return 0;
}