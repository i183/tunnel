project(tunnel C)

set(CMAKE_C_STANDARD 99)

add_executable(tunneld tunneld.c)
add_executable(tunnel tunnel.c server/server.c server/server.h)