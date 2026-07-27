#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/spin.hpp>

namespace exe::thread {

class SpinLock {
 public:
  void Lock() {
    while (locked_.exchange(true)) {
      while (locked_.load()) {
      }
    }
  }

  bool TryLock() {
    return !locked_.exchange(true);
  }

  void Unlock() {
    locked_.store(false);
  }

  // Lockable

  void lock() {  // NOLINT
    Lock();
  }

  bool try_lock() {  // NOLINT
    return TryLock();
  }

  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  twist::ed::std::atomic_bool locked_{false};
};

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
    twist::ed::std::atomic<Guard*> next_ = nullptr;
    twist::ed::std::atomic_bool is_owner_ = false;
  };

 private:
  void Acquire(Guard* waiter) {
    twist::ed::SpinWait spin_wait;

    auto head = head_.exchange(waiter);
    if (head == nullptr) {
      waiter->is_owner_ = true;
    } else {
      head->next_ = waiter;

      while (!waiter->is_owner_) {
        spin_wait();
      };
    }
  }

  void Release(Guard* owner) {
    twist::ed::SpinWait spin_wait;

    while (owner->next_.load() == nullptr) {
      auto current_owner = owner;
      if (head_.compare_exchange_strong(current_owner, nullptr)) {
        return;
      }
      spin_wait();
    }

    owner->next_.load()->is_owner_.store(true);
  }

 private:
  twist::ed::std::atomic<Guard*> head_ = nullptr;
};

}  // namespace exe::thread
