# Tunnel
Tunnel是一款单线程、轻量级和高性能的内网穿透程序，支持TCP流量转发（支持任何TCP上层协议，包括HTTP，SSH等）。

解决公网无法访问内网网络服务问题，使内网网络服务能被公网访问。

## 说明
- 该项目使用CMake构建，Linux系统编译后会产生两个可执行文件，tunneld（服务器端）和tunnel（客户端），其他系统只会产生tunnel（客户端）。
- tunneld考虑性能原因，使用epoll I/O模型，仅支持Linux系统，且Linux内核版本要大于等于2.5.44。
- tunnel使用select I/O模型，支持主流系统（Mac OS、Windows、Linux）。
- 公网映射端口为动态分配，暂时不支持指定端口。

## tunneld（服务器端）使用说明
执行 tunneld 文件
```
$ tunneld
$ Enter listening port(default 8877):8877
$ Enter connection password(default maqian.cc):maqian.cc
$ Port:8877, Password:maqian.cc
```
这时 tunneld 已启动成功，等待客户端连接

## tunnel（客户端）使用说明
执行 tunnel 文件
```
$ tunnel
$ Enter server IP:xx.xx.xxx.xxx
$ Enter server port(default 8877):8877
$ Enter connection password(default maqian.cc):maqian.cc
$ Enter local port:22
$ Server IP:xx.xx.xxx.xxx, Server Port:8877, Password:maqian.cc, Local Port:22
$ Successfully connected, address: xx.xx.xxx.xxx:45915
```
这样 tunnel 就连接服务器成功了，address是映射到公网的IP和端口，通过访问address的IP和端口就可以访问内网网络服务了。