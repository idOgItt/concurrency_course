#pragma once
#include "channel_node.hpp"
#include <exe/unit.hpp>

namespace exe::fiber {
namespace detail {

template <typename SelectWaiter, typename Clause>
class ClauseWaiter final : public detail::ChannelNode<typename Clause::T> {
 public:
  using T = typename Clause::T;
  using R = typename Clause::ReturnType;
  using Receiver = std::is_same<R, T>;

  explicit ClauseWaiter(Clause clause, size_t num, SelectWaiter* waiter)
      : clause_(std::move(clause)),
        num_(num),
        waiter_(waiter) {
  }

  // Channel node interface
  Role GetRole() const override {
    return clause_.GetRole();
  }

  bool Fire() override {
    return waiter_->Fire();
  }

  void Emplace(T value) override {
    clause_.Emplace(std::move(value));
  }

  T Value() override {
    return clause_.Value();
  }

  void Notify() override {
    waiter_->Notify(num_);
  }

  // Utility
  void Lock() {
    clause_.Lock();
  }

  void Unlock() {
    clause_.Unlock();
  }

  bool Try() {
    return clause_.Try(*waiter_);
  }

  void Push() {
    clause_.Push(this);
  }

  size_t Priority() const {
    return clause_.Priority();
  }

  uintptr_t ChanPtr() const {
    return clause_.ChanPtr();
  }

  void Abort() {
    clause_.Abort(this);
  }

  R GetResult() {
    if constexpr (Receiver::value) {
      return clause_.Value();
    } else {
      return unit;
    }
  }

 private:
  Clause clause_;
  size_t num_;
  SelectWaiter* waiter_;
};

}  // namespace detail
}  // namespace exe::fiber
