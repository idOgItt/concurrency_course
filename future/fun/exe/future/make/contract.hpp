#pragma once

#include <exe/future/type/future.hpp>

#include <exe/future/detail/contract_state.hpp>
#include <exe/runtime/inline.hpp>

#include <cassert>
#include <cstdlib>
#include <tuple>

namespace exe::future {

template <typename T>
class Future;

template <typename T>
class Promise {
  template <typename U>
  friend std::tuple<Future<U>, Promise<U>> Contract(runtime::View);

  using InnerState = detail::ContractState<T>;

 public:
  Promise(Promise&& other)
      : state_(std::exchange(other.state_, nullptr)) {
  }

  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;
  Promise& operator=(Promise&&) = delete;

  ~Promise() {
    if (state_) {
      state_->Release();
    }
  }

  void Set(T val) && {
    assert(state_ != nullptr);
    auto* s = std::exchange(state_, nullptr);
    s->SetValue(std::move(val));
    s->Release();
  }

 private:
  explicit Promise(InnerState* state)
      : state_(state) {
  }

  InnerState* state_;
};

template <typename T>
std::tuple<Future<T>, Promise<T>> Contract(runtime::View view) {
  auto* ptr = new detail::ContractState<T>();
  return {Future<T>(ptr, view), Promise<T>(ptr)};
}

template <typename T>
std::tuple<Future<T>, Promise<T>> Contract() {
  return Contract<T>(runtime::Inline());
}

}  // namespace exe::future
