#include <elog/logger.h>
#include <netpoll/core.h>

#include <atomic>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <vector>

#include "tiemr.h"

using namespace netpoll;
enum class ShowDetail;

int        s_port{-1};
int        s_numThread{-1};
int        s_numConnection{-1};
int        s_numRandomData{-1};
ShowDetail s_showDetail;

std::atomic_int64_t s_connectionCount{0};
std::atomic_int64_t s_connectionFailedNum{0};
std::atomic_int64_t s_sumDuration{0};
std::string         s_randomData;

std::mutex            s_mtx;
std::vector<uint64_t> s_queue;
enum class ShowDetail { ALL = 0, DATA, PER_ECHO, AVG_P99, SUM };

const char* s_details[static_cast<size_t>(ShowDetail::SUM)] = {
  "all", "data", "per_echo", "avg_p99"};

void RegisterAndRun();
bool ParseInt(std::string const& src, int& dst);
bool ParseConfig(int argc, char** argv);

void GenRandomData();

struct Context
{
   ::Metrics   metrics;
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
      ctx.metrics.end();
      ctx.text += buffer->readAll();
      // 如果所有回声数据接收完毕
      if (ctx.text == s_randomData)
      {
         auto tt = ctx.metrics.get_duration().count();
         s_sumDuration += tt;
         {
            std::lock_guard<std::mutex> guard(s_mtx);
            if (s_showDetail == ShowDetail::ALL ||
                s_showDetail == ShowDetail::PER_ECHO)
               ctx.metrics.print_duration("Get all random duration: ");
            s_queue.push_back(tt);
         }
         conn->forceClose();
      }
      ctx.metrics.start();
   }

   NETPOLL_TCP_CONNECTION_ERROR() { ++s_connectionFailedNum; }
};

int main(int argc, char** argv)
{
   if (!ParseConfig(argc, argv))
   {
      std::cout << "Please input some args: \narg1:port\narg2:thread "
                   "num\narg3:connection num\narg4:random data "
                   "length\narg5:show detail(`all`|`data`|`per_echo`|`avg_p99`)"
                << std::endl;
      return -1;
   }
   elog::GlobalConfig::Get().setLevel(elog::kError);
   GenRandomData();
   if (s_showDetail == ShowDetail::ALL || s_showDetail == ShowDetail::DATA)
      std::cout << "Random data:" << s_randomData << std::endl;
   ::Metrics tm;
   RegisterAndRun();
   tm.end();

   int64_t echoSyncSum = s_sumDuration;
   int64_t avg = echoSyncSum / (s_numConnection - s_connectionFailedNum);
   int64_t p99 = s_queue[s_queue.size() * 0.99];   // NOLINT

   if (s_showDetail == ShowDetail::ALL || s_showDetail == ShowDetail::AVG_P99)
      std::cout << "Echo sync:\nsum:" << echoSyncSum << "ns("
                << (double)echoSyncSum / 1000000
                << "ms)\n"
                   "avg:"
                << avg << "ns(" << (double)avg / 1000000 << "ms)\n";
   std::sort(s_queue.begin(), s_queue.end());
   if (s_showDetail == ShowDetail::ALL || s_showDetail == ShowDetail::AVG_P99)
      std::cout << "p99:" << p99 << "ns(" << (double)p99 / 1000000 << "ms)\n";

   int64_t echoAsyncSum = tm.get_duration().count();
   int64_t echoAsyncAvg =
     echoAsyncSum / (s_numConnection - s_connectionFailedNum);

   if (s_showDetail == ShowDetail::ALL || s_showDetail == ShowDetail::AVG_P99)
      std::cout << "Echo async:\nsum:" << echoAsyncSum << "ns("
                << (double)echoAsyncSum / 1000000 << "ms)\n"
                << "avg:" << echoAsyncAvg << "ns("
                << (double)echoAsyncAvg / 1000000 << "ms)\n";
   if (s_connectionFailedNum > 0)
   {
      std::cout << "Rejected num:" << s_connectionFailedNum << "\n";
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
   std::vector<netpoll::tcp::Dialer> dialers;
   for (int i = 0; i < s_numConnection; i++)
   {
      auto dialer =
        netpoll::tcp::Dialer::New({"127.0.0.1", static_cast<uint16_t>(s_port)});
      dialer.bind<EchoClient>();
      dialers.push_back(std::move(dialer));
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

bool ParseShow(const std::string& src, ShowDetail& detail)
{
   auto cast_size_t = [](size_t s) { return static_cast<ShowDetail>(s); };
   for (size_t i = 0; i < static_cast<size_t>(ShowDetail::SUM); i++)
   {
      if (src == s_details[i])
      {
         detail = cast_size_t(i);
         return true;
      }
   }
   return false;
}

bool ParseConfig(int argc, char** argv)
{
   if (argc != 6) return false;

   return ParseInt(argv[1], s_port) && ParseInt(argv[2], s_numThread) &&
          ParseInt(argv[3], s_numConnection) &&
          ParseInt(argv[4], s_numRandomData) &&
          ParseShow(argv[5], s_showDetail);
}
