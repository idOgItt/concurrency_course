#pragma once

#include "tagged_semaphore.hpp"

#include <deque>

// Bounded Blocking Multi-Producer/Multi-Consumer (MPMC) Queue

template <typename T>
class BoundedBlockingQueue {
 public:
  explicit BoundedBlockingQueue(size_t capacity)
      : capacity_(capacity),
        available_space_tagged_semaphore_(capacity),
        available_item_tagged_semaphore_(0) {
  }

  void Put(T item) {
    auto space_permit = available_space_tagged_semaphore_.Acquire();
    {
      twist::ed::std::unique_lock<twist::ed::std::mutex> lock(mutex_);
      queue_.push_back(std::move(item));
    }
    available_item_tagged_semaphore_.Release(std::move(space_permit));
  }

  T Take() {
    auto item_permit = available_item_tagged_semaphore_.Acquire();
    T item;
    {
      twist::ed::std::unique_lock<twist::ed::std::mutex> lock(mutex_);
      item = std::move(queue_.front());
      queue_.pop_front();
    }

    available_space_tagged_semaphore_.Release(std::move(item_permit));
    return item;
  }

 private:
  struct SpaceTag {};
  struct ItemTag {};
  struct STag {};

 private:
  size_t capacity_;
  std::deque<T> queue_;
  twist::ed::std::mutex mutex_;

  TaggedSemaphore<STag> available_space_tagged_semaphore_;
  TaggedSemaphore<STag> available_item_tagged_semaphore_;
};
