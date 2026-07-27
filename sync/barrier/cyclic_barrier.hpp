#pragma once

#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/condition_variable.hpp>

#include <cstddef>

class CyclicBarrier {
 public:
  explicit CyclicBarrier(size_t participants)
      : participants_(participants),
        counter_(0),
        phase_(0) {
  }

  void ArriveAndWait() {
    twist::ed::std::unique_lock<twist::ed::std::mutex> lock(mutex_);
    size_t current_phase = phase_;

    if (++counter_ == participants_) {
      ++phase_;
      counter_ = 0;
      condition_barrier_.notify_all();
    } else {
      condition_barrier_.wait(lock, [this, current_phase] {
        return current_phase != phase_;
      });
    }
  }

 private:
  size_t participants_;
  size_t counter_ = 0;
  size_t phase_;
  twist::ed::std::condition_variable condition_barrier_;
  twist::ed::std::mutex mutex_;
};
