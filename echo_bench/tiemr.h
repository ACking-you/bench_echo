#pragma once

#include <chrono>
#include <iostream>

class Timer
{
public:
   Timer() { m_StartTimepoint = std::chrono::high_resolution_clock::now(); }

   long long Stop(const char* description)
   {
      auto endTimepoint = std::chrono::high_resolution_clock::now();
      auto start = std::chrono::time_point_cast<std::chrono::microseconds>(
                     m_StartTimepoint)
                     .time_since_epoch()
                     .count();
      auto end =
        std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint)
          .time_since_epoch()
          .count();
      auto   duration = end - start;        // 以微秒为单位
      double ms       = duration * 0.001;   // 得到毫秒
      printf("%s: %ld us (%lf ms)\n", description, duration, ms);
      fflush(stdout);
      return duration;
   }

private:
   std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
};
