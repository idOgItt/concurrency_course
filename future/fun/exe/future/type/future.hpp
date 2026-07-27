#pragma once

#include <cassert>
#include <exe/runtime/view.hpp>
#include <exe/future/detail/contract_state.hpp>
#include <exe/future/make/contract.hpp>

namespace exe::future {

template <typename T>
class Promise;

template <typename T>
class [[nodiscard]] Future {
  template <typename U>
  friend std::tuple<Future<U>, Promise<U>> Contract(runtime::View);

  using InnerState = detail::ContractState<T>;

 public:
  using ValueType = T;

  Future(Future&& other)
      : state_(std::exchange(other.state_, nullptr)),
        rt_(other.rt_) {
  }

  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;
  Future& operator=(Future&&) = delete;

  ~Future() {
    if (state_) {
      state_->Release();
    }
  }

  void Consume(detail::Callback<T> cb) && {
    assert(state_ != nullptr);
    auto* s = std::exchange(state_, nullptr);

    s->SetCallback(std::move(cb));
    s->Release();
  }

  runtime::View Runtime() {
    return rt_;
  }

  void SetRuntime(runtime::View rt) {
    rt_ = rt;
  }

 private:
  explicit Future(InnerState* state, runtime::View rt)
      : state_(state),
        rt_(rt) {
  }

  InnerState* state_;
  runtime::View rt_;
};

}  // namespace exe::future
