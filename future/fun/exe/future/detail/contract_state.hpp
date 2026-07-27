#pragma once

#include "callback.hpp"
#include <twist/ed/std/atomic.hpp>

namespace exe::future {

namespace detail {

template <typename T>
class ContractState {
  union Control {
    struct {
      uint32_t status;
      uint32_t refs;
    } parts;
    uint64_t combined;
  };

  enum FsmState : uint32_t {
    kInit = 0,
    kHasValue = 1,
    kHasCallback = 2,
    kReady = 3
  };

 public:
  ContractState() {
    Control initial{{kInit, 2}};
    state_.store(initial);
  }

  void Release() {
    Control prev = state_.load();
    Control next;

    do {
      next = prev;
      next.parts.refs--;
    } while (!state_.compare_exchange_weak(prev, next));

    if (next.parts.refs == 0) {
      delete this;
    }
  }

  void SetValue(T val) {
    payload_.emplace(std::move(val));

    FsmState old = UpdateState(kHasValue, kReady);
    if (old == kHasCallback) {
      Run();
    }
  }

  void SetCallback(Callback<T> cb) {
    continuation_.emplace(std::move(cb));

    FsmState old = UpdateState(kHasCallback, kReady);
    if (old == kHasValue) {
      Run();
    }
  }

 private:
  FsmState UpdateState(FsmState on_init, FsmState otherwise) {
    Control prev = state_.load();
    Control next;

    do {
      next = prev;
      if (prev.parts.status == kInit) {
        next.parts.status = on_init;
      } else {
        next.parts.status = otherwise;
      }
    } while (!state_.compare_exchange_weak(prev, next));

    return static_cast<FsmState>(prev.parts.status);
  }

  void Run() {
    assert(payload_.has_value() && continuation_.has_value());
    (*continuation_)(std::move(*payload_));
  }

 private:
  twist::ed::std::atomic<Control> state_{{}};
  std::optional<T> payload_;
  std::optional<Callback<T>> continuation_;
};

}  // namespace detail

}  // namespace exe::future
