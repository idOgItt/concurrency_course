#pragma once

#include <exe/fiber/sync/wait_group.hpp>
#include <exe/thread/spinlock.hpp>
#include <vvv/list.hpp>

namespace exe::fiber::detail {

template <typename T>
class RendezvousChannelState {
  struct Waiter : vvv::IntrusiveListNode<Waiter> {
    FiberHandle fiber_handle;
    std::optional<T> payload;

    explicit Waiter(FiberHandle fh)
        : fiber_handle(fh) {
    }

    explicit Waiter(FiberHandle fh, T val)
        : fiber_handle(fh),
          payload(std::move(val)) {
    }
  };

 public:
  RendezvousChannelState() = default;

  ~RendezvousChannelState() {
    assert(send_queue_.IsEmpty());
    assert(receive_queue_.IsEmpty());
  }

  void Send(T value) {
    spin_lock_.lock();

    if (auto* waiting_receiver = receive_queue_.TryPopFront()) {
      waiting_receiver->payload.emplace(std::move(value));
      waiting_receiver->fiber_handle.Resume();
      spin_lock_.unlock();
      return;
    }

    Waiter sender_node{Fiber::Handle(), std::move(value)};
    send_queue_.PushBack(&sender_node);

    fiber::Suspend([&] {
      spin_lock_.unlock();
    });
  }

  T Receive() {
    spin_lock_.lock();

    if (auto* waiting_sender = send_queue_.TryPopFront()) {
      T result = std::move(waiting_sender->payload.value());
      waiting_sender->fiber_handle.Resume();
      spin_lock_.unlock();
      return result;
    }

    Waiter receiver_node{Fiber::Handle()};
    receive_queue_.PushBack(&receiver_node);

    fiber::Suspend([&] {
      spin_lock_.unlock();
    });
    return std::move(receiver_node.payload.value());
  }

 private:
  thread::SpinLock spin_lock_;
  vvv::IntrusiveList<Waiter> send_queue_;
  vvv::IntrusiveList<Waiter> receive_queue_;
};

}  // namespace exe::fiber::detail
