#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/futex.hpp>

#include <cstdint>

class CondVar {
 public:
  template <class Mutex>
  void Wait(Mutex& mutex) {
    uint32_t old_gen = generation_.load();
    mutex.unlock();
    if (generation_.load() == old_gen) {
      twist::ed::futex::Wait(generation_, old_gen);
    }
    mutex.lock();
  }

  void NotifyOne() {
    generation_.fetch_add(1);
    auto key = twist::ed::futex::PrepareWake(generation_);
    twist::ed::futex::WakeOne(key);
  }

  void NotifyAll() {
    generation_.fetch_add(1);
    auto key = twist::ed::futex::PrepareWake(generation_);
    twist::ed::futex::WakeAll(key);
  }

 private:
  twist::ed::std::atomic<uint32_t> generation_{0};
};