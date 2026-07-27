#pragma once

#include <twist/ed/std/lock_guard.hpp>
#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/spin.hpp>

namespace exe::thread {

class SpinLock {
 public:
  void Lock() {
    while (flag_.exchange(true)) {
      while (flag_.load()) {
      }
    }
  }

  bool TryLock() {
    return !flag_.exchange(true);
  }

  void Unlock() {
    flag_.store(false);
  }

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
  twist::ed::std::atomic_bool flag_{false};
};

using LockGuard = twist::ed::std::lock_guard<SpinLock>;

}  // namespace exe::thread
