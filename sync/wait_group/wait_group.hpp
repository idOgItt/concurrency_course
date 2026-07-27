#pragma once

#include <cstddef>
#include <twist/ed/std/condition_variable.hpp>
#include <twist/ed/std/mutex.hpp>

class WaitGroup {
 public:
  WaitGroup()
      : counter_(0),
        phase_(0),
        waiters_(0) {
  }

  void Add(const size_t count) {
    std::lock_guard<twist::ed::std::mutex> lock(mutex_);

    counter_ += count;
  }

  void Done() {
    std::lock_guard<twist::ed::std::mutex> lock(mutex_);
    if (--counter_ == 0) {
      phase_++;
      if (waiters_ > 0) {
        condition_.notify_all();
      }
    }
  }

  void Wait() {
    std::unique_lock<twist::ed::std::mutex> lock(mutex_);
    ++waiters_;
    condition_.wait(lock, [this] {
      return counter_ == 0;
    });
    --waiters_;

    ++phase_;
  }

 private:
  size_t counter_;
  size_t phase_;
  size_t waiters_;
  twist::ed::std::mutex mutex_;
  twist::ed::std::condition_variable condition_;
};
