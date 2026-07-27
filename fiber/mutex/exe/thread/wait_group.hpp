#pragma once

#include <cstddef>
#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/condition_variable.hpp>
#include <twist/ed/std/unique_lock.hpp>

namespace exe::thread {

class WaitGroup {
 public:
  void Add(size_t count) {
    twist::ed::std::lock_guard lock{mutex_};
    counter_.fetch_add(count);
  }

  void Done() {
    twist::ed::std::lock_guard lock{mutex_};

    auto todo = counter_.fetch_sub(1);
    if (todo == 1 && waiters_.load() > 0) {
      done_.notify_all();
    }
  }

  void Wait() {
    twist::ed::std::unique_lock lock{mutex_};

    waiters_.fetch_add(1);
    while (counter_.load() != 0) {
      done_.wait(lock);
    }
    waiters_.fetch_sub(1);
  }

 private:
  twist::ed::std::atomic<uint32_t> counter_{0};
  twist::ed::std::atomic<uint32_t> waiters_{0};
  twist::ed::std::condition_variable done_;
  twist::ed::std::mutex mutex_;
};

}  // namespace exe::thread
