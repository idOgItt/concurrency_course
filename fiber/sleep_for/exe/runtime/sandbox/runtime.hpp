#pragma once

#include "clock.hpp"
#include "timer_queue.hpp"
#include "exe/runtime/timer/scheduler.hpp"
#include "exe/runtime/task/scheduler.hpp"

#include <queue>
#include <exe/runtime/view.hpp>

#include <exe/runtime/timer/duration.hpp>

namespace exe::runtime::sandbox {

class Runtime : public task::IScheduler,
                public timer::IScheduler {
 public:
  Runtime() = default;

  // NOLINTNEXTLINE
  operator View() {
    return {this, this};
  }

  void Submit(task::Task task) override {
    tasks_.push(std::move(task));
  }

  void Set(timer::Duration delay, task::Task handler) override {
    timer_queue_.Push(clock_.Now() + delay, std::move(handler));
  }

  // Run

  // Tasks

  size_t RunAtMostTasks(size_t limit);

  bool RunNextTask() {
    return RunAtMostTasks(1) == 1;
  }

  size_t RunTasks();

  // Timers

  size_t AdvanceClockBy(timer::Duration delta);

  size_t AdvanceClockToNextDeadline();

  // Empty

  bool IsEmpty() const;

  bool NonEmpty() const {
    return !IsEmpty();
  }

 private:
  Clock clock_;
  TimerQueue timer_queue_;
  std::queue<task::Task> tasks_;
};

}  // namespace exe::runtime::sandbox
