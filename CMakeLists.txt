project(tunnel C)

set(CMAKE_C_STANDARD 99)

add_executable(tunneld tunneld.c common/global.h common/global.c server/server.c server/server.h)
add_executable(tunnel tunnel.c)