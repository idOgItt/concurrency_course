#pragma once

#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/condition_variable.hpp>

class Semaphore {
 public:
  explicit Semaphore(const size_t coins)
      : counter_(coins) {
  }

  void Acquire() {
    twist::ed::std::unique_lock<twist::ed::std::mutex> lock(mutex_);
    condition_.wait(lock, [this] {
      return counter_ > 0;
    });
    --counter_;
  }

  void Release() {
    twist::ed::std::unique_lock<twist::ed::std::mutex> lock(mutex_);
    ++counter_;
    condition_.notify_one();
  }

 private:
  __int64_t counter_;
  twist::ed::std::mutex mutex_;
  twist::ed::std::condition_variable condition_;
};
