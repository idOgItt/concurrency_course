#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/spin.hpp>

/*
 * Scalable Queue SpinLock
 *
 * Usage:
 *
 * QueueSpinLock spinlock;
 *
 * {
 *   QueueSpinLock::Guard guard{spinlock};  // <-- Acquire spinlock
 *   // <-- Critical section
 * }  // <-- Release spinlock (~Guard)
 *
 */
namespace exe::sync {
class QueueSpinLock {
 public:
  class Guard {
    friend class QueueSpinLock;

   public:
    explicit Guard(QueueSpinLock& host)
        : host_(host) {
      host_.Acquire(this);
    }

    // Non-copyable
    Guard(const Guard&) = delete;
    Guard& operator=(const Guard&) = delete;

    // Non-movable
    Guard(Guard&&) = delete;
    Guard& operator=(Guard&&) = delete;

    ~Guard() {
      host_.Release(this);
    }

   private:
    QueueSpinLock& host_;
    twist::ed::std::atomic_bool is_owner_{false};
    twist::ed::std::atomic<Guard*> next_{nullptr};
  };

 private:
  void Acquire(Guard* waiter) {
    auto prev = tail_.exchange(waiter);

    if (prev != nullptr) {
      prev->next_.store(waiter);

      do {
        if (spin_.ConsiderParking()) {
          spin_();
        }
      } while (!waiter->is_owner_.load());
    } else {
      waiter->is_owner_.store(true);
    }
  }

  void Release(Guard* owner) {
    Guard* next = owner->next_.load();

    if (next == nullptr) {
      Guard* expected = owner;
      if (tail_.compare_exchange_strong(expected, nullptr)) {
        return;
      }

      do {
        if (spin_.ConsiderParking()) {
          spin_();
        }
        next = owner->next_.load();
      } while (next == nullptr);
    }

    next->is_owner_.store(true);
  }

 private:
  twist::ed::std::atomic<Guard*> tail_{nullptr};
  twist::ed::SpinWait spin_;
};
}  // namespace exe::sync