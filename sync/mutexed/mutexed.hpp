#pragma once

#include <twist/ed/std/mutex.hpp>
#include <utility>
#include <mutex>

template <typename T, class Mutex = twist::ed::std::mutex>
class Mutexed {
 public:
  template <typename... Args>
  explicit Mutexed(Args&&... args)
      : object_(std::forward<Args>(args)...) {
  }

  class OwnerRef {
   public:
    OwnerRef(T& object, Mutex& mutex)
        : object_(object),
          lock_(mutex) {
    }

    T* operator->() {
      return &object_;
    }

    T& operator*() {
      return object_;
    }

   private:
    T& object_;
    std::lock_guard<Mutex> lock_;
  };

  OwnerRef Acquire() {
    return OwnerRef(object_, mutex_);
  }

 private:
  T object_;
  Mutex mutex_;
};

template <typename T, typename Mutex>
auto Acquire(Mutexed<T, Mutex>& object) {
  return object.Acquire();
}
