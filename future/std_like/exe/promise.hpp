#pragma once

#include "error.hpp"
#include "future.hpp"

#include <memory>

template <typename T>
class Promise {
 public:
  Promise()
      : shared_state_(std::make_shared<typename Future<T>::SharedState>()) {
  }

  // Non-copyable
  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;

  // Movable
  Promise(Promise&&) = default;
  Promise& operator=(Promise&&) = default;

  ~Promise() {
    if (shared_state_) {
      twist::ed::std::unique_lock<twist::ed::std::mutex> lock(
          shared_state_->mutex);
      if (!shared_state_->valid) {
        shared_state_->result = std::expected<T, std::exception_ptr>(
            std::unexpected(std::make_exception_ptr(BrokenPromiseError())));
        shared_state_->valid = true;
        shared_state_->condition.notify_all();
      }
    }
  }

  // One-shot
  Future<T> MakeFuture() {
    if (!shared_state_) {
      throw NoStateError();
    }
    return Future<T>(shared_state_);
  }

  // One-shot
  void SetValue(T value) {
    if (!shared_state_) {
      throw NoStateError();
    }

    twist::ed::std::unique_lock<twist::ed::std::mutex> lock(
        shared_state_->mutex);
    if (shared_state_->valid) {
      throw PromiseAlreadySatisfiedError();
    }

    shared_state_->result =
        std::expected<T, std::exception_ptr>{std::move(value)};
    shared_state_->valid = true;
    shared_state_->condition.notify_all();
  }

  // One-shot
  void SetException(std::exception_ptr error) {
    if (!shared_state_) {
      throw NoStateError();
    }

    twist::ed::std::unique_lock<twist::ed::std::mutex> lock(
        shared_state_->mutex);
    if (shared_state_->valid) {
      throw PromiseAlreadySatisfiedError();
    }
    shared_state_->result = std::unexpected(error);
    shared_state_->valid = true;
    shared_state_->condition.notify_all();
  }

 private:
  std::shared_ptr<typename Future<T>::SharedState> shared_state_;
};
