#pragma once

#include "lock-free_timer_queue.hpp"
#include "twist/ed/std/condition_variable.hpp"

#include <exe/runtime/timer/scheduler.hpp>
#include <exe/runtime/task/scheduler.hpp>
#include "twist/ed/std/mutex.hpp"

#include <twist/ed/std/thread.hpp>
#include <twist/ed/std/chrono.hpp>

namespace exe::runtime::multi_thread {

class TimerThread final : public timer::IScheduler {
 private:
  using Clock = twist::ed::std::chrono::steady_clock;
  using TimePoint = Clock::time_point;

  struct Timer {
    TimePoint deadline_;
    timer::Handler handler_;

    bool operator<(const Timer& other) const {
      return deadline_ < other.deadline_;
    }

    bool operator>(const Timer& other) const {
      return deadline_ > other.deadline_;
    }
  };

 public:
  explicit TimerThread(task::IScheduler*);
  ~TimerThread();

  // timer::IScheduler
  void Set(timer::Duration /*delay*/, timer::Handler) override;

  void Start();
  void Stop();

 private:
  void WorkerRoutine();
  void FetchNewTimers(twist::ed::std::unique_lock<twist::ed::std::mutex>&);
  void WaitForNewTimers(twist::ed::std::unique_lock<twist::ed::std::mutex>&);
  void ExecuteReadyTimer(twist::ed::std::unique_lock<twist::ed::std::mutex>&);

 private:
  task::IScheduler* scheduler_;
  TimerQueue timer_queue_;

  std::vector<Timer> heap_;
  twist::ed::std::mutex mutex_;
  twist::ed::std::condition_variable condition_;
  twist::ed::std::thread worker_;
  bool stopped_{false};
};

}  // namespace exe::runtime::multi_thread
