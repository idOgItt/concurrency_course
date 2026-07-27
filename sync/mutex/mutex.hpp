#pragma once

#include "wheels/core/assert.hpp"

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/spin.hpp>
#include <twist/ed/wait/futex.hpp>

class Mutex {
 public:
  void Lock() {
    twist::ed::SpinWait spin_wait;

    while (!TryLock()) {
      if (spin_wait.ConsiderParking()) {
        waiters_.fetch_add(1);
        ParkThisWaiter();
      } else {
        spin_wait();
      }
    }
  }

  void Unlock() {
    WHEELS_VERIFY(locked_.exchange(0) == 1, "Mutex is not locked");
    if (waiters_.load() > 0) {
      auto wake_key = twist::ed::futex::PrepareWake(locked_);
      twist::ed::futex::WakeOne(wake_key);
    }
  }

  void lock() {  // NOLINT
    Lock();
  }

  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  twist::ed::std::atomic<uint32_t> locked_{0};
  twist::ed::std::atomic<uint32_t> waiters_{0};

  bool TryLock() {
    return locked_.exchange(1) == 0u;
  }

  void ParkThisWaiter() {
    twist::ed::futex::Wait(locked_, 1);
  }
};
