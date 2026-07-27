#pragma once

#include <cassert>
#include <cstddef>
#include <memory>

template <typename T>
class CircularBuffer {
  struct alignas(T) Storage {
    std::byte data[sizeof(T)];
  };

 public:
  explicit CircularBuffer(size_t capacity)
      : capacity_(capacity),
        buffer_(std::make_unique<Storage[]>(capacity)) {
  }

  ~CircularBuffer() {
    while (!IsEmpty()) {
      Pop();
    }
  }

  bool IsEmpty() const {
    return size_ == 0;
  }

  bool IsFull() const {
    return size_ == capacity_;
  }

  void Push(T item) {
    assert(!IsFull());

    new (&buffer_[tail_]) T(std::move(item));
    tail_ = (tail_ + 1) % capacity_;
    size_++;
  }

  T Pop() {
    assert(!IsEmpty());

    T* item_ptr = std::launder(reinterpret_cast<T*>(&buffer_[head_]));
    T item = std::move(*item_ptr);

    item_ptr->~T();
    head_ = (head_ + 1) % capacity_;
    size_--;
    return item;
  }

 private:
  const size_t capacity_;
  std::unique_ptr<Storage[]> buffer_;
  size_t head_{0};
  size_t tail_{0};
  size_t size_{0};
};
