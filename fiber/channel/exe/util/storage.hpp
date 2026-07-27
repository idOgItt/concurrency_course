#pragma once

#include <new>
#include <utility>

namespace exe::util {

template <typename T>
class Storage {
  alignas(T) unsigned char buf_[sizeof(T)];
  bool has_{false};

 public:
  Storage() = default;

  explicit Storage(T&& v) {
    new (&buf_) T(std::move(v));
    has_ = true;
  }

  ~Storage() {
    if (has_) {
      reinterpret_cast<T*>(&buf_)->~T();
    }
  }

  template <typename... Args>
  void Emplace(Args&&... args) {
    Reset();
    new (&buf_) T(std::forward<Args>(args)...);
    has_ = true;
  }

  T& operator*() {
    return *reinterpret_cast<T*>(&buf_);
  }

  const T& operator*() const {
    return *reinterpret_cast<const T*>(&buf_);
  }

  Storage& operator=(T&& v) {
    if (has_) {
      reinterpret_cast<T*>(&buf_)->~T();
    }
    new (&buf_) T(std::move(v));
    has_ = true;
    return *this;
  }

  void Reset() {
    if (has_) {
      (**this).~T();
      has_ = false;
    }
  }
};

}  // namespace exe::util
