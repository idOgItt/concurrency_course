#pragma once

#include "spinlock.hpp"

namespace exe::thread {

// Простейшая реализация UniqueLock для спинлока.
// Захватывает мьютекс в конструкторе и отпускает в деструкторе или по вызову
// Unlock().
template <typename Mutex>
class UniqueLock {
 public:
  explicit UniqueLock(Mutex& m)
      : mtx_(&m),
        owns_(true) {
    mtx_->Lock();
  }

  ~UniqueLock() {
    if (owns_) {
      mtx_->Unlock();
    }
  }

  // Запрещаем копирование
  UniqueLock(const UniqueLock&) = delete;
  UniqueLock& operator=(const UniqueLock&) = delete;

  // Разрешаем перемещение
  UniqueLock(UniqueLock&& other) noexcept
      : mtx_(other.mtx_),
        owns_(other.owns_) {
    other.mtx_ = nullptr;
    other.owns_ = false;
  }
  UniqueLock& operator=(UniqueLock&& other) noexcept {
    if (this != &other) {
      if (owns_ && mtx_) {
        mtx_->Unlock();
      }
      mtx_ = other.mtx_;
      owns_ = other.owns_;
      other.mtx_ = nullptr;
      other.owns_ = false;
    }
    return *this;
  }

  // Явно отпустить мьютекс раньше деструктора
  void Unlock() {
    if (owns_ && mtx_) {
      mtx_->Unlock();
      owns_ = false;
    }
  }

 private:
  Mutex* mtx_;
  bool owns_;
};

}  // namespace exe::thread
