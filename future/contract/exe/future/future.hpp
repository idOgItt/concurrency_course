#pragma once

#include "detail/shared_state.hpp"

#include <cassert>

namespace exe::future {

template <typename T>
class Promise;

template <typename T>
class Future {
  friend class Promise<T>;

  template <typename U>
  friend std::tuple<Future<U>, Promise<U>> Contract();

  using State = detail::SharedState<T>;

 public:
  using ValueType = T;

  Future(Future&& other)
      : state_(other.state_) {
    other.state_ = nullptr;
  }

  ~Future() {
    if (state_) {
      state_->Release();
    }
  }

  void Consume(Callback<T> callback) && {
    assert(state_ != nullptr);
    auto* state = std::exchange(state_, nullptr);

    state->SetCallback(std::move(callback));
    state->Release();
  }

 private:
  explicit Future(State* state)
      : state_(state) {
  }

  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;
  Future& operator=(Future&&) = delete;

 private:
  State* state_;
};

}  // namespace exe::future
