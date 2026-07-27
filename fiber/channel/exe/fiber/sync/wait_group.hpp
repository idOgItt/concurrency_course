#pragma once

#include <cstddef>

#include <exe/fiber/core/fiber.hpp>
#include <exe/fiber/core/handle.hpp>
#include <exe/thread/spinlock.hpp>
#include <exe/fiber/sched/suspend.hpp>
#include <vvv/list.hpp>

namespace exe::fiber {

class WaitGroup {
  using GuardLock = twist::ed::std::lock_guard<thread::SpinLock>;

  struct Waiter : vvv::IntrusiveListNode<Waiter> {
    FiberHandle fiberHandle_;
    explicit Waiter(FiberHandle h)
        : fiberHandle_(h) {
    }
  };

 public:
  void Add(std::size_t delta) {
    GuardLock guard{mutex_};
    counter_ += delta;
  }

  void Done() {
    vvv::IntrusiveList<Waiter> resume_list;
    {
      GuardLock guard{mutex_};
      --counter_;
      if (counter_ != 0) {
        return;
      }
      resume_list.Append(waiters_list_);
    }
    while (auto* w = resume_list.TryPopFront()) {
      w->fiberHandle_.Resume();
    }
  }

  void Wait() {
    GuardLock guard{mutex_};
    if (counter_ == 0) {
      return;
    }
    Waiter w{Fiber::Handle()};
    waiters_list_.PushBack(&w);
    fiber::Suspend([&] {
      mutex_.unlock();
    });
  }

 private:
  thread::SpinLock mutex_;
  std::size_t counter_{0};
  vvv::IntrusiveList<Waiter> waiters_list_;
};

}  // namespace exe::fiber
