#pragma once

#include "detail/shared_state.hpp"

#include <cassert>

namespace exe::future {

template <typename T>
class Future;

template <typename T>
class Promise {
  template <typename U>
  friend std::tuple<Future<U>, Promise<U>> Contract();

  using State = detail::SharedState<T>;

 public:
  Promise(Promise&& other)
      : state_(other.state_) {
    other.state_ = nullptr;
  }

  ~Promise() {
    if (state_) {
      state_->Release();
    }
  }

  void Set(T value) && {
    assert(state_ != nullptr);
    auto* state = std::exchange(state_, nullptr);

    state->SetValue(std::move(value));
    state->Release();
  }

 private:
  explicit Promise(State* state)
      : state_(state) {
  }

  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;
  Promise& operator=(Promise&&) = delete;

 private:
  State* state_;
};

}  // namespace exe::future
