#pragma once

#include <deque>
#include <mutex>
#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/condition_variable.hpp>

#include <optional>

// Unbounded blocking multi-producers/multi-consumers (MPMC) queue

template <typename T>
class UnboundedBlockingQueue {
 public:
  void Push(T el) {
    std::lock_guard lock{mutex_};

    deque_.push_back(std::move(el));
    not_empty_.notify_all();
  }

  std::optional<T> Pop() {
    std::unique_lock lock{mutex_};

    while (!closed_ && deque_.empty()) {
      not_empty_.wait(lock);
    }

    if (!deque_.empty()) {
      T el = std::move(deque_.front());
      deque_.pop_front();
      return el;
    } else {
      return std::nullopt;
    }
  }

  void Close() {
    std::lock_guard lock{mutex_};

    closed_ = true;
    not_empty_.notify_all();
  }

 private:
  std::deque<T> deque_;
  twist::ed::std::mutex mutex_;
  twist::ed::std::condition_variable not_empty_;
  bool closed_{false};
};
