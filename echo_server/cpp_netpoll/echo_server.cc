#include <netpoll/core.h>

struct server
{
   NETPOLL_TCP_MESSAGE(conn, buffer) { conn->send(buffer->readAll()); }
};

int main()
{
   auto loop = netpoll::NewEventLoop(std::thread::hardware_concurrency() + 1);
   auto listener = netpoll::tcp::Listener::New({7777});
   listener.enableKickoffIdle(5);
   listener.bind<server>();
   loop.serve(listener);
}