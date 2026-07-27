#pragma once

#include "mutex.hpp"

namespace exe {

class LockGuard {
 public:
  explicit LockGuard(Mutex& mutex)
      : mutex_(mutex) {
    mutex_.Lock();
  }

  ~LockGuard() {
    mutex_.Unlock();
  }

  LockGuard(const LockGuard&) = delete;
  LockGuard& operator=(const LockGuard&) = delete;

 private:
  Mutex& mutex_;
};

}  // namespace exe
