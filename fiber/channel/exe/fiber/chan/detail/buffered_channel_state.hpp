#pragma once

#include <cassert>
#include "circular_buffer.hpp"
#include <exe/fiber/core/fiber.hpp>
#include <exe/fiber/sched/suspend.hpp>
#include <exe/thread/spinlock.hpp>
#include <vvv/list.hpp>

namespace exe::fiber::detail {

template <typename T>
class BufferedChannelState {
  using LockGuard = std::lock_guard<thread::SpinLock>;

  struct Waiter : vvv::IntrusiveListNode<Waiter> {
    FiberHandle fiber_handle;

    explicit Waiter(FiberHandle fh)
        : fiber_handle(fh) {
    }
  };

 public:
  explicit BufferedChannelState(size_t capacity)
      : circ_buffer_(capacity) {
    assert(capacity > 0);
  }

  ~BufferedChannelState() {
    assert(send_queue_.IsEmpty());
    assert(receive_queue_.IsEmpty());
  }

  void Send(T value) {
    LockGuard lock_guard{spin_lock_};

    while (circ_buffer_.IsFull()) {
      Waiter waiter_node{Fiber::Handle()};
      send_queue_.PushBack(&waiter_node);

      Suspend([this] {
        spin_lock_.unlock();
      });
      spin_lock_.lock();
    }

    circ_buffer_.Push(std::move(value));

    if (auto* waiting_receiver = receive_queue_.TryPopFront()) {
      waiting_receiver->fiber_handle.Resume();
    }
  }

  T Receive() {
    LockGuard lock_guard{spin_lock_};

    while (circ_buffer_.IsEmpty()) {
      Waiter waiter_node{Fiber::Handle()};
      receive_queue_.PushBack(&waiter_node);

      Suspend([this] {
        spin_lock_.unlock();
      });
      spin_lock_.lock();
    }

    T result = circ_buffer_.Pop();

    if (auto* waiting_sender = send_queue_.TryPopFront()) {
      waiting_sender->fiber_handle.Resume();
    }

    return result;
  }

 private:
  thread::SpinLock spin_lock_;
  CircularBuffer<T> circ_buffer_;
  vvv::IntrusiveList<Waiter> send_queue_;
  vvv::IntrusiveList<Waiter> receive_queue_;
};

}  // namespace exe::fiber::detail
