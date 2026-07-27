#pragma once

#include <exe/fiber/core/fiber.hpp>
#include <exe/thread/spinlock.hpp>
#include <exe/fiber/sched/suspend.hpp>
#include <vvv/list.hpp>

namespace exe::fiber {

class Mutex {
  using LockGuard = twist::ed::std::lock_guard<thread::SpinLock>;

  struct Waiter : vvv::IntrusiveListNode<Waiter> {
    FiberHandle handle;

    explicit Waiter(FiberHandle handle)
        : handle(handle) {};
  };

 public:
  void Lock() {
    lock_.lock();

    if (!locked_) {
      locked_ = true;
      lock_.unlock();
      return;
    }

    auto waiter = Waiter(Fiber::Handle());
    waiting_queue_.PushBack(&waiter);

    fiber::Suspend([&] {
      lock_.unlock();
    });
  }

  bool TryLock() {
    LockGuard guard{lock_};

    if (!locked_) {
      locked_ = true;
      return true;
    }
    return false;
  }

  void Unlock() {
    LockGuard guard{lock_};  // тут надо тоже как в event и wg?

    locked_ = false;
    if (auto waiter = waiting_queue_.TryPopFront()) {
      waiter->handle.Resume();
      locked_ = true;
    }
  }

  // Lockable

  void lock() {  // NOLINT
    Lock();
  }

  bool try_lock() {  // NOLINT
    return TryLock();
  }

  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  thread::SpinLock lock_;
  bool locked_{false};
  vvv::IntrusiveList<Waiter> waiting_queue_;
};

}  // namespace exe::fiber
