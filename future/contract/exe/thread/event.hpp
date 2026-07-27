#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/futex.hpp>

namespace exe::thread {

class Event {
 public:
  void Fire() {
    state_.store(kFired);

    auto key = twist::ed::futex::PrepareWake(state_);
    twist::ed::futex::WakeAll(key);
  }

  void Wait() {
    while (state_.load() != kFired) {
      twist::ed::futex::Wait(state_, kWaiting);
    }
  }

 private:
  static const uint32_t kWaiting = 0;
  static const uint32_t kFired = 1;

  twist::ed::std::atomic<uint32_t> state_{kWaiting};
};

}  // namespace exe::thread
