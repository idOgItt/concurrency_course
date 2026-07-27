#pragma once

#include "error.hpp"

#include <memory>
#include <exception>
#include <optional>
#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/condition_variable.hpp>
#include <__expected/expected.h>

template <typename T>
class Future {
  template <typename U>
  friend class Promise;

 public:
  // Non-copyable
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  // Movable
  Future(Future&&) = default;
  Future& operator=(Future&&) = default;

  // One-shot
  T Get() {
    if (!shared_state_) {
      throw NoStateError();
    }

    twist::ed::std::unique_lock<twist::ed::std::mutex> lock(
        shared_state_->mutex);
    shared_state_->condition.wait(lock, [this] {
      return shared_state_->valid;
    });

    auto local_result = std::move(shared_state_->result);
    lock.unlock();
    shared_state_.reset();

    if (!local_result.has_value()) {
      throw BrokenPromiseError();
    }
    if (local_result->has_value()) {
      return std::move(local_result->value());
    }
    std::rethrow_exception(local_result->error());
  }

  bool Valid() const {
    return shared_state_ != nullptr;
  }

 private:
  struct SharedState {
    twist::ed::std::mutex mutex;
    twist::ed::std::condition_variable condition;
    bool valid = false;
    std::optional<std::expected<T, std::exception_ptr>> result;
  };

  explicit Future(std::shared_ptr<SharedState> state)
      : shared_state_(std::move(state)) {
  }

 private:
  std::shared_ptr<SharedState> shared_state_;
};
