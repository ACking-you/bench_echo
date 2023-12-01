
find_package(Boost REQUIRED)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif ()

add_executable(asio_echo_server echo_server.cc)
target_link_libraries(asio_echo_server ${Boost_LIBRARIES})
