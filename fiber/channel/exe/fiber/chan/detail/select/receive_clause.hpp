#pragma once

#include "clause.hpp"
#include <optional>

namespace exe::fiber {
namespace detail {

template <typename ChannelState>
class ReceiveClause final : public Clause<ChannelState> {
 public:
  using T = typename Clause<ChannelState>::T;
  using ReturnType = T;
  using State = ChannelState;

  ReceiveClause(std::weak_ptr<ChannelState> chan)  // NOLINT
      : Clause<State>(chan) {
  }

  Role GetRole() const override {
    return Role::Consumer;
  }

  void Emplace(T value) override {
    value_.emplace(std::move(value));
  }

  T Value() override {
    return std::move(value_.value());
  }

  template <typename Unlockable>
  bool Try(Unlockable& unlocker) {
    if (auto value_opt = this->TryReceive(unlocker)) {
      value_.emplace(std::move(*value_opt));
      return true;
    }
    return false;
  }

  size_t Priority() const {
    return this->Receive();
  }

 private:
  std::optional<T> value_;
};

}  // namespace detail
}  // namespace exe::fiber