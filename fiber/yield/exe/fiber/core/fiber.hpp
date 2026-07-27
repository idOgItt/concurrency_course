#pragma once

#include "body.hpp"
#include "coroutine.hpp"
#include "scheduler.hpp"

#include <memory>
#include <atomic>

namespace exe::fiber {

class Fiber : public std::enable_shared_from_this<Fiber> {
 public:
  Fiber(Scheduler& sched, Body body);

  void Resume();
  void Yield();

  Scheduler* GetScheduler() const {
    return scheduler_;
  }
  static Fiber& Self();

 private:
  void Schedule();

 private:
  Scheduler* scheduler_;
  Coroutine coroutine_;
  twist::ed::std::atomic_bool scheduled_{false};
};

using FiberPtr = std::shared_ptr<exe::fiber::Fiber>;

}  // namespace exe::fiber
