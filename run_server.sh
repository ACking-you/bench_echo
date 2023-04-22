#!/bin/bash
if [ $# -eq 0 ]; then
  echo "Usage: $0 <netpoll-cpp|netpoll-go|asio>"
  exit 1
fi

case $1 in
  "netpoll-cpp")
    echo "Starting the netpoll-cpp on prot 7777..."
    cd build/echo_server/cpp_netpoll&&./netpoll_echo_server
    ;;
  "netpoll-go")
    echo "Starting the netpoll-go on port 9999..."
    cd echo_server/go_netpoll && go run main.go
    ;;
  "asio")
    echo "Starting the asio on port 8888..."
    cd build/echo_server/cpp_asio&&./asio_echo_server
    # 执行重启服务的命令
    ;;
  *)
    echo "Unknown command: $1"
    exit 1
    ;;
esac

exit 0
