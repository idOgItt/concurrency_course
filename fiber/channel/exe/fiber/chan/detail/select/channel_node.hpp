#pragma once
#include "vvv/list.hpp"
namespace exe::fiber {
namespace detail {

enum class Role {
  Producer,
  Consumer,
};

template <typename T>
struct ChannelNode : public ::vvv::IntrusiveListNode<ChannelNode<T>> {
  virtual Role GetRole() const = 0;
  virtual bool Fire() = 0;
  virtual void Emplace(T value) = 0;
  virtual T Value() = 0;
  virtual void Notify() = 0;
};

}  // namespace detail
}  // namespace exe::fiber
