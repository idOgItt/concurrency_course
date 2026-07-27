#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/spin.hpp>
#include <twist/ed/wait/futex.hpp>

class Mutex {
 public:
  void Lock() {
    twist::ed::SpinWait spin_wait;

    if (uint32_t expected = 0; state_.compare_exchange_strong(expected, 1)) {
      return;
    }

    while (!TryLock()) {
      if (spin_wait.ConsiderParking()) {
        ParkThisWaiter();
      } else {
        spin_wait();
      }
    }
  }

  void Unlock() {
    if (state_.exchange(0) == 2) {
      const auto wake_key = twist::ed::futex::PrepareWake(state_);
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
  twist::ed::std::atomic<uint32_t> state_{0};

  bool TryLock() {
    return state_.exchange(2) == 0;
  }

  void ParkThisWaiter() {
    twist::ed::futex::Wait(state_, 2);
  }
};
