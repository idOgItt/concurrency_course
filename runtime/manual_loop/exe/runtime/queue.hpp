#pragma once

#include <optional>
#include <queue>
#include "mutex.hpp"
#include "mutex_guard.hpp"
#include "condvar.hpp"

template <typename T>
class UnboundedBlockingQueue {
 public:
  void Push(T value) {
    {
      exe::LockGuard lock(mutex_);
      queue_.push(std::move(value));
    }
    cv_.NotifyOne();
  }

  std::optional<T> Pop() {
    exe::LockGuard lock(mutex_);
    while (queue_.empty() && !closed_) {
      cv_.Wait(mutex_);
    }
    if (queue_.empty() && closed_) {
      return std::nullopt;
    }
    T value = std::move(queue_.front());
    queue_.pop();
    return value;
  }

  void Close() {
    {
      exe::LockGuard lock(mutex_);
      closed_ = true;
    }
    cv_.NotifyAll();
  }

 private:
  std::queue<T> queue_;
  bool closed_{false};
  Mutex mutex_;
  CondVar cv_;
};
