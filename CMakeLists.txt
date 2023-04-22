cmake_minimum_required(VERSION 3.14)
project(bench_echo)
include(cmake/CPM.cmake)
set(CMAKE_CXX_STANDARD 17)

CPMAddPackage(
        NAME netpoll
        GIT_REPOSITORY https://gitee.com/acking-you/netpoll-cpp.git
        GIT_TAG origin/master
        GIT_SHALLOW TRUE
)

#echo server实现
add_subdirectory(echo_server/cpp_asio)
add_subdirectory(echo_server/cpp_netpoll)

#echo bench实现
add_subdirectory(echo_bench)