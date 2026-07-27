#pragma once

#include <cstddef>
#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/condition_variable.hpp>
#include <twist/ed/std/unique_lock.hpp>

namespace exe::thread {

class WaitGroup {
 public:
  void Add(size_t delta) {
    twist::ed::std::lock_guard lock{sync_};
    tasks_.fetch_add(delta);
  }

  void Done() {
    twist::ed::std::lock_guard lock{sync_};

    auto remaining = tasks_.fetch_sub(1);
    if (remaining == 1 && sleepers_.load() > 0) {
      signal_.notify_all();
    }
  }

  void Wait() {
    twist::ed::std::unique_lock lock{sync_};

    sleepers_.fetch_add(1);
    while (tasks_.load() != 0) {
      signal_.wait(lock);
    }
    sleepers_.fetch_sub(1);
  }

 private:
  twist::ed::std::atomic<uint32_t> tasks_{0};
  twist::ed::std::atomic<uint32_t> sleepers_{0};
  twist::ed::std::condition_variable signal_;
  twist::ed::std::mutex sync_;
};

}  // namespace exe::thread
