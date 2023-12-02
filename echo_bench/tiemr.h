#pragma once

#include <chrono>
#include <iostream>

class Metrics
{
public:
   using time_point_t = std::chrono::steady_clock::time_point;
   using duration_t   = std::chrono::steady_clock::duration;
   Metrics()
     : current_time_(std::chrono::steady_clock::now()),
       duration_(std::chrono::steady_clock::duration::zero())
   {
   }

   duration_t get_duration() { return duration_; }

   void print_duration(const char* description)
   {
      std::cout << description << duration_.count() << std::endl;
   }

   class ClockGuard
   {
   private:
      Metrics& metrics;

   public:
      ClockGuard(Metrics& m) : metrics(m) { metrics.start(); }

      ~ClockGuard() { metrics.end(); }
   };

   ClockGuard clock_guard() { return {*this}; }

   time_point_t start()
   {
      current_time_ = std::chrono::steady_clock::now();
      return current_time_;
   }

   time_point_t end()
   {
      auto end_time = std::chrono::steady_clock::now();
      duration_ += end_time - current_time_;
      return end_time;
   }

private:
   time_point_t current_time_;
   duration_t   duration_;
};
