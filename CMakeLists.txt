project(tunnel C)

set(CMAKE_C_STANDARD 99)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
    message(STATUS ">>> Linux")
else()
    message(STATUS ">>> Not Linux, Can't compile tunneld, cause platform not supported.")
endif()

add_executable(tunneld tunneld.c common/global.h common/global.c server/server.c server/server.h server/tunnel.c server/tunnel.h server/common.c server/common.h server/listener.c server/listener.h common/command.h structure/queue.h structure/queue.c server/cmd.c server/cmd.h structure/array.h structure/array.c)
add_executable(tunnel tunnel.c common/command.h common/global.h common/global.c structure/queue.h structure/queue.c client/client.c client/client.h client/os.c client/os.h structure/array.h structure/array.c client/common.c client/common.h)