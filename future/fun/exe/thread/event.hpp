#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/futex.hpp>

namespace exe::thread {

class Event {
 public:
  void Fire() {
    state_.store(kSet, std::memory_order_release);
    auto futex_key = twist::ed::futex::PrepareWake(state_);
    twist::ed::futex::WakeAll(futex_key);
  }

  void Wait() {
    while (state_.load(std::memory_order_acquire) != kSet) {
      twist::ed::futex::Wait(state_, kInit);
    }
  }

 private:
  static const uint32_t kInit = 0;
  static const uint32_t kSet = 1;

  twist::ed::std::atomic_uint32_t state_{kInit};
};

}  // namespace exe::thread
