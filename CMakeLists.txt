project(tunnel C)

set(CMAKE_C_STANDARD 99)

add_executable(tunnels server.c)
add_executable(tunnel client.c)