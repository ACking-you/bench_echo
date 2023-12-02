#!/bin/bash
set -e
if [ "$#" -ne 3 ]; then
    echo "Please input some args:\narg1:connection pool thread num\narg2:connection num\narg3:random data length"
    exit 1
fi
echo "--------------netpoll-cpp--------------"
sh run_bench.sh 7777 $1 $2 $3 avg_p99
echo "--------------asio--------------"
sh run_bench.sh 8888 $1 $2 $3 avg_p99
echo "--------------netpoll-go--------------"
sh run_bench.sh 9999 $1 $2 $3 avg_p99
echo "--------------rust-tokio"--------------
sh run_bench.sh 11111 $1 $2 $3 avg_p99
echo "--------------rust-monoio--------------"
sh run_bench.sh 22222 $1 $2 $3 avg_p99