#include <netpoll/core.h>

struct server
{
   NETPOLL_TCP_MESSAGE(conn, buffer) { conn->send(buffer->readAll()); }
};

int main()
{
   auto loop     = netpoll::NewEventLoop(2);
   auto listener = netpoll::tcp::Listener::New({7777});
   listener->bind<server>();
   loop.serve(listener);
}