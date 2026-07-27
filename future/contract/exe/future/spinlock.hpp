#pragma once

#include <atomic>
#include "twist/ed/std/atomic.hpp"
namespace exe::thread {

class SpinLock {
 public:
  void Lock() {
    while (locked_.exchange(true, std::memory_order_acquire)) {
      while (locked_.load(std::memory_order_relaxed)) {
        ;
      }
    }
  }

  bool TryLock() {
    return !locked_.exchange(true, std::memory_order_acquire);
  }

  void Unlock() {
    locked_.store(false, std::memory_order_release);
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
  twist::ed::std::atomic_bool locked_{false};
};

}  // namespace exe::thread
