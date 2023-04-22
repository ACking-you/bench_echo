cmake -DCMAKE_BUILD_TYPE=Release -B build -S .
cd build &&make asio_echo_server -j4&&make netpoll_echo_server -j4&&make bench_echo -j4