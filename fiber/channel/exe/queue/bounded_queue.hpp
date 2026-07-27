#pragma once

#include "exe/util/storage.hpp"

#include <cstddef>
#include <vector>
#include <cassert>

namespace exe {
namespace queue {

template <typename T>
class BoundedQueue {
 public:
  explicit BoundedQueue(std::size_t capacity)
      : cap_(capacity),
        data_(capacity),
        head_(0),
        tail_(0),
        size_(0) {
    assert(capacity > 0);
  }

  bool IsFull() const noexcept {
    return size_ == cap_;
  }

  bool IsEmpty() const noexcept {
    return size_ == 0;
  }

  void Push(T&& value) {
    data_[tail_] = std::move(value);
    tail_ = (tail_ + 1) % cap_;
    ++size_;
  }

  T Pop() {
    assert(size_ > 0);
    T tmp = std::move(data_[head_]);
    head_ = (head_ + 1) % cap_;
    --size_;
    return tmp;
  }

 private:
  const std::size_t cap_;
  std::vector<util::Storage<T>> data_;
  std::size_t head_, tail_, size_;
};

}  // namespace queue
}  // namespace exe
