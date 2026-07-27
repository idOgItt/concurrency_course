#include <algorithm>
#include <twist/ed/std/atomic.hpp>
#include <vector>

template <typename D, typename T>
class MSPCQueue {
 private:
  struct HeapElement {
    D deadline_;
    T task_;

    HeapElement(D deadline, T data)
        : deadline_(deadline),
          task_(std::move(data)) {};

    bool operator<(const HeapElement& other) const {
      return deadline_ > other.deadline_;  // min heap shenanegans
    }
  };

  struct Node {
    D deadline;
    T task_;
    twist::ed::std::atomic<Node*> next_{nullptr};

    Node(D deadline, T data)
        : deadline(deadline),
          task_(std::move(data)) {};
  };

 public:
  void Push(D deadline, T data) {
    Node* node = new Node(deadline, std::move(data));

    Node* old_head = head_.load();
    do {
      node->next_.store(old_head);
    } while (!head_.compare_exchange_strong(old_head, node));
  };

  D PeekFirstDeadline() {
    Collect();

    return heap_.front().deadline_;
  }

  T PopFirstTask() {
    std::pop_heap(heap_.begin(), heap_.end());
    auto item = std::move(heap_.back());
    heap_.pop_back();
    return std::move(item.task_);
  }

  bool Empty() {
    return heap_.empty() && !head_;
  }

  void Collect() {
    Node* node = head_.exchange(nullptr);

    while (node) {
      heap_.emplace_back(node->deadline, std::move(node->task_));
      Node* temp = node;
      node = node->next_.load();
      delete temp;
    }

    if (!heap_.empty()) {
      std::make_heap(heap_.begin(), heap_.end());
    }
  }

 private:
  twist::ed::std::atomic<Node*> head_{nullptr};
  std::vector<HeapElement> heap_;
};
