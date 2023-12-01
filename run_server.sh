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
    ;;
  "rust-tokio")
    echo "Starting the rust-tokio on port 11111..."
    cd build/echo_server/rust_tokio_monoio&&cargo +nightly run --bin tokio_echo --release
    ;;
  "rust-monoio")
    echo "Starting the rust-monoio on port 22222..."
    cd build/echo_server/rust_tokio_monoio&&cargo +nightly run --bin monoio_echo --release
    ;;
  *)
    echo "Unknown command: $1"
    exit 1
    ;;
esac

exit 0
