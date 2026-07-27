#pragma once

#include <exe/fiber/core/fiber.hpp>
#include <exe/thread/spinlock.hpp>
#include <exe/fiber/sched/suspend.hpp>
#include <vvv/list.hpp>

namespace exe::fiber {

// One-shot
class Event {
  using LockGuard = twist::ed::std::lock_guard<thread::SpinLock>;

  struct Waiter : vvv::IntrusiveListNode<Waiter> {
    FiberHandle fiberHandle_;

    explicit Waiter(FiberHandle h)
        : fiberHandle_(h) {
    }
  };

 public:
  void Wait() {
    mutex_.lock();

    if (signaled_) {
      mutex_.unlock();
      return;
    }

    auto node = Waiter(Fiber::Handle());
    listeners_.PushBack(&node);

    fiber::Suspend([&] {
      mutex_.unlock();
    });
  }

  void Fire() {
    vvv::IntrusiveList<Waiter> resume_queue;

    {
      LockGuard guard{mutex_};
      signaled_ = true;
      resume_queue.Append(listeners_);
    }

    while (auto* node = resume_queue.TryPopFront()) {
      node->fiberHandle_.Resume();
    }
  }

 private:
  thread::SpinLock mutex_;
  bool signaled_{false};
  vvv::IntrusiveList<Waiter> listeners_;
};

}  // namespace exe::fiber
