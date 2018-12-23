# Tunnel
Tunnel是一款单线程、轻量级和高性能的内网穿透程序，支持TCP流量转发（支持任何TCP上层协议）。该项目包括tunneld（服务器）和tunnel（客户端）

### tunneld（服务器）
tunneld使用epoll IO模型，仅支持Linux系统。

### tunnel（客户端）
tunnel考虑到系统兼容性，使用select IO模型，支持主流系统（Mac OS、Windows、Linux）