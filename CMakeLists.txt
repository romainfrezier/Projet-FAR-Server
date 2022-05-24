cmake_minimum_required(VERSION 3.21)
project(Server C)

set(CMAKE_C_STANDARD 99)

add_executable(Server server.c lib/src/colors.c lib/src/tools.c lib/src/list.c lib/src/stringFunc.c lib/src/commandServer.c lib/src/sema.c lib/src/fileServer.c lib/src/admin.c lib/src/channel.c lib/src/channelList.c)

