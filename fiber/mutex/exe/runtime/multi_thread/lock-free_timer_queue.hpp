#pragma once

#include "twist/ed/std/chrono.hpp"
#include "twist/trace/attr/twist.hpp"
#include "../timer/handler.hpp"
#include "twist/ed/std/atomic.hpp"

namespace exe::runtime::multi_thread {
class TimerQueue {
 private:
  using Clock = twist::ed::std::chrono::high_resolution_clock;
  using TimePoint = Clock::time_point;

  struct Node {
    TimePoint deadline_;
    timer::Handler handler_;
    twist::ed::std::atomic<Node*> next_{nullptr};

    Node(TimePoint deadline, timer::Handler handler)
        : deadline_(deadline),
          handler_(std::move(handler)) {
    }
  };

 public:
  TimerQueue() {
    dummy_ = new Node(TimePoint(), {});
    head_.store(dummy_);
    tail_.store(dummy_);
  }

  ~TimerQueue() {
    Node* node = head_.load();
    while (node != nullptr) {
      Node* next = node->next_.load();
      delete node;
      node = next;
    }
  }

  void Push(TimePoint deadline, timer::Handler handler) {
    Node* node = new Node(deadline, std::move(handler));
    Node* prev_tail = tail_.exchange(node);
    prev_tail->next_.store(node);
  }

  // std::vector<timer::Handler> PopReady(TimePoint now) {
  //   std::vector<timer::Handler> ready;
  //
  //   Node* prev = head_.load();
  //   Node* curr = prev->next_.load();
  //
  //   while (curr) {
  //     if (curr->deadline_ <= now) {
  //       ready.push_back(std::move(curr->handler_));
  //       head_.store(curr);
  //       delete prev;
  //       prev = curr;
  //       curr = curr->next_.load();
  //     } else {
  //       break;
  //     }
  //   }
  //   return ready;
  // }
  //
  // std::optional<TimePoint> NextDeadline() const {
  //   Node* next = head_.load()->next.load();
  //   if (!next) {
  //     return std::nullopt;
  //   }
  //   return next->deadline;
  // }

  std::vector<std::pair<TimePoint, timer::Handler>> PopAll() {
    std::vector<std::pair<TimePoint, timer::Handler>> result;

    Node* prev = head_.load();
    Node* curr = prev->next_.load();

    while (curr != nullptr) {
      result.emplace_back(curr->deadline_, std::move(curr->handler_));
      head_.store(curr);
      delete prev;
      prev = curr;
      curr = curr->next_.load();
    }

    return result;
  }

  bool IsEmpty() const {
    return head_.load()->next_.load() == nullptr;
  }

 private:
  twist::ed::std::atomic<Node*> head_{nullptr};
  twist::ed::std::atomic<Node*> tail_{nullptr};
  Node* dummy_;
};
}  // namespace exe::runtime::multi_thread
