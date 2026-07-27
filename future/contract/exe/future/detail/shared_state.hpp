#pragma once

#include <exe/future/callback.hpp>
#include <twist/ed/std/atomic.hpp>

namespace exe::future::detail {

template <typename T>
class SharedState {
  union State {
    struct {
      uint32_t state_machine_state_;
      uint32_t ref_count;
    } parts;
    uint64_t whole;
  };

  enum FsmState : uint32_t {
    kInit = 0,
    kProducer = 1,
    kConsumer = 2,
    kRendezvous = 3
  };

 public:
  SharedState() {
    State initial{{kInit, 2}};
    state_.store(initial);
  }

  void Release() {
    State old_state = state_.load();
    State new_state;

    do {
      new_state = old_state;
      new_state.parts.ref_count--;
    } while (!state_.compare_exchange_weak(old_state, new_state));

    if (new_state.parts.ref_count == 0) {
      delete this;
    }
  }

  void SetValue(T value) {
    value_.emplace(std::move(value));

    FsmState prev_state = TransitionState(kProducer, kRendezvous);
    if (prev_state == kConsumer) {
      ExecuteCallback();
    }
  }

  void SetCallback(Callback<T> callback) {
    callback_.emplace(std::move(callback));

    FsmState prev_state = TransitionState(kConsumer, kRendezvous);
    if (prev_state == kProducer) {
      ExecuteCallback();
    }
  }

 private:
  FsmState TransitionState(FsmState from_init, FsmState from_other) {
    State old_state = state_.load();
    State new_state;

    do {
      new_state = old_state;
      if (old_state.parts.state_machine_state_ == kInit) {
        new_state.parts.state_machine_state_ = from_init;
      } else {
        new_state.parts.state_machine_state_ = from_other;
      }
    } while (!state_.compare_exchange_weak(old_state, new_state));

    return static_cast<FsmState>(old_state.parts.state_machine_state_);
  }

  void ExecuteCallback() {
    assert(value_.has_value() && callback_.has_value());
    (*callback_)(std::move(*value_));
  }

 private:
  twist::ed::std::atomic<State> state_{{}};
  std::optional<T> value_;
  std::optional<Callback<T>> callback_;
};

}  // namespace exe::future::detail
