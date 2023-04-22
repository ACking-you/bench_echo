#include <elog/logger.h>
#include <netpoll/core.h>

#include <atomic>
#include <iostream>
#include <random>
#include <string>

#include "tiemr.h"

using namespace netpoll;

int s_port{-1};
int s_numThread{-1};
int s_numConnection{-1};
int s_numRandomData{-1};

std::atomic_int64_t s_connectionCount{0};
std::atomic_int64_t s_connectionFailedNum{0};
std::atomic_int64_t s_sumDuration{0};
std::string         s_randomData;

void RegisterAndRun();
bool ParseInt(std::string const& src, int& dst);
bool ParseConfig(int argc, char** argv);

void GenRandomData();

struct Context
{
   ::Timer     timer;
   std::string text;
};

struct EchoClient
{
   NETPOLL_TCP_CONNECTION(conn)
   {
      if (conn->connected())
      {
         conn->send(s_randomData);
         conn->setContext(Context{});
      }
      ++s_connectionCount;
      if (conn->disconnected())
      {
         if (s_connectionCount.load() ==
             (s_numConnection - s_connectionFailedNum.load()) * 2)
         {
            netpoll::QuitAllEventLoop();
         }
      }
   }

   NETPOLL_TCP_MESSAGE(conn, buffer)
   {
      auto& ctx = netpoll::any_cast<Context&>(conn->getMutableContext());
      ctx.text += buffer->readAll();
      // 如果所有回声数据接收完毕
      if (ctx.text == s_randomData)
      {
         s_sumDuration += ctx.timer.Stop("echo expand");
         conn->forceClose();
      }
   }

   NETPOLL_TCP_CONNECTION_ERROR() { ++s_connectionFailedNum; }
};

int main(int argc, char** argv)
{
   ::Timer tm;
   if (!ParseConfig(argc, argv))
   {
      std::cout << "Please input some args: \narg1:port\narg2:thread "
                   "num\narg3:connection num\narg4:random data length"
                << std::endl;
      return 0;
   }
   elog::GlobalConfig::Get().setLevel(elog::kError);
   GenRandomData();
   std::cout << "Random data:" << s_randomData << std::endl;
   RegisterAndRun();
   std::cout << "all echo sync:\nsum:" << s_sumDuration << "us\naverage:"
             << s_sumDuration / (s_numConnection - s_connectionFailedNum)
             << "us\n";
   std::cout << "all echo async:\n";
   auto sum = tm.Stop("sum:");
   std::cout << "average:" << sum / (s_numConnection - s_connectionFailedNum)
             << "us\n";
   if (s_connectionFailedNum > 0)
   {
      std::cout << "The rejected connection num:" << s_connectionFailedNum
                << "\n";
   }
}

void GenRandomData()
{
   std::random_device              rd;
   std::mt19937                    gen(rd());
   std::uniform_int_distribution<> dis('a', 'z');

   // 生成随机字符
   for (int i = 0; i < s_numRandomData; i++)
   {
      char ch = static_cast<char>(dis(gen));
      s_randomData.push_back(ch);
   }
}

void RegisterAndRun()
{
   auto loop = netpoll::NewEventLoop(s_numThread);
   loop.enableQuit();
   std::vector<netpoll::tcp::DialerPtr> dialers;
   for (int i = 0; i < s_numConnection; i++)
   {
      auto dialer =
        netpoll::tcp::Dialer::New({"127.0.0.1", static_cast<uint16_t>(s_port)});
      dialer->bind<EchoClient>();
      dialers.push_back(dialer);
   }
   loop.serve(dialers);
}

bool ParseInt(const std::string& src, int& dst)
{
   size_t endIdx;
   dst = std::stoi(src, &endIdx, 10);
   if (endIdx != src.size()) { return false; }
   if (dst < 0) { return false; }
   return true;
}

bool ParseConfig(int argc, char** argv)
{
   if (argc != 5) return false;

   return ParseInt(argv[1], s_port) && ParseInt(argv[2], s_numThread) &&
          ParseInt(argv[3], s_numConnection) &&
          ParseInt(argv[4], s_numRandomData);
}
