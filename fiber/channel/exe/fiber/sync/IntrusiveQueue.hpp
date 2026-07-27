#pragma once

#include "IntrusiveNode.hpp"
#include "twist/ed/std/atomic.hpp"
#include <concepts>
#include <cassert>

namespace exe::sync {

template <typename Node>
concept IntrusiveNodeConcept = std::derived_from<Node, IntrusiveNode>;

template <IntrusiveNodeConcept Node>
class IntrusiveQueue {
 public:
  IntrusiveQueue()
      : head_(nullptr),
        tail_(nullptr) {
    auto* dummy = new Node();
    dummy->next.store(nullptr, std::memory_order_relaxed);
    head_.store(dummy, std::memory_order_relaxed);
    tail_.store(dummy, std::memory_order_relaxed);
  }

  ~IntrusiveQueue() {
    while (auto* node = PopFront()) {
      delete node;
    }
    delete head_.load();
  }

  void PushBack(Node* node) {
    node->next.store(nullptr, std::memory_order_relaxed);

    while (true) {
      Node* tail = tail_.load(std::memory_order_acquire);
      auto* next = tail->next.load(std::memory_order_acquire);

      if (next == nullptr) {
        auto* expected = static_cast<IntrusiveNode*>(nullptr);
        if (tail->next.compare_exchange_weak(expected, node,
                                             std::memory_order_release,
                                             std::memory_order_relaxed)) {
          tail_.compare_exchange_weak(tail, node, std::memory_order_release,
                                      std::memory_order_relaxed);
          return;
        }
      } else {
        tail_.compare_exchange_weak(tail, static_cast<Node*>(next),
                                    std::memory_order_release,
                                    std::memory_order_relaxed);
      }
    }
  }

  Node* PopFront() {
    Node* const sentinel = head_.load(std::memory_order_acquire);

    while (true) {
      auto* first = sentinel->next.load(std::memory_order_acquire);
      if (first == nullptr) {
        return nullptr;
      }

      Node* tail = tail_.load(std::memory_order_acquire);
      if (static_cast<Node*>(first) == tail) {
        tail_.compare_exchange_weak(tail, sentinel, std::memory_order_release,
                                    std::memory_order_relaxed);
      }

      auto* next =
          static_cast<Node*>(first)->next.load(std::memory_order_acquire);

      if (sentinel->next.compare_exchange_weak(first, next,
                                               std::memory_order_release,
                                               std::memory_order_relaxed)) {
        return static_cast<Node*>(first);
      }
    }
  }

 private:
  twist::ed::std::atomic<Node*> head_;
  twist::ed::std::atomic<Node*> tail_;
};

}  // namespace exe::sync
