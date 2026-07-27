#pragma once

#include "channel_node.hpp"
#include <memory>
#include <optional>

namespace exe::fiber {
namespace detail {

template <typename ChannelState>
class Clause {
 public:
  using T = typename ChannelState::Type;
  using State = ChannelState;

  Clause(std::weak_ptr<ChannelState> chan)  // NOLINT
      : chan_(std::move(chan)) {
  }

  void Lock() {
    if (auto ch = chan_.lock()) {
      return ch->Lock();
    }
  }

  void Unlock() {
    if (auto ch = chan_.lock()) {
      return ch->Unlock();
    }
  }

  template <typename Unlockable>
  bool TrySend(T value, Unlockable& unlocker) {
    if (auto ch = chan_.lock()) {
      return ch->TrySend(std::move(value), unlocker);
    }
    return false;
  }

  template <typename Unlockable>
  std::optional<T> TryReceive(Unlockable& unlocker) {
    if (auto ch = chan_.lock()) {
      return ch->TryReceive(unlocker);
    }
    return {};
  }

  void Push(ChannelNode<T>* node) {
    if (auto ch = chan_.lock()) {
      return ch->Push(node);
    }
  }

  size_t Received() const {
    if (auto ch = chan_.lock()) {
      return ch->Received();
    }
    return static_cast<size_t>(-1);
  }

  size_t Sent() const {
    if (auto ch = chan_.lock()) {
      return ch->Sent();
    }
    return static_cast<size_t>(-1);
  }

  uintptr_t ChanPtr() const {
    if (auto ch = chan_.lock()) {
      return ch->ChanPtr();
    }
    return 0;
  }

 private:
  std::weak_ptr<ChannelState> chan_;
};

}  // namespace detail
}  // namespace exe::fiber