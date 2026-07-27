#pragma once

#include "future.hpp"
#include "promise.hpp"

#include <tuple>

namespace exe::future {

template <typename T>
std::tuple<Future<T>, Promise<T>> Contract() {
  auto* state_ptr = new detail::SharedState<T>();
  return {Future<T>(state_ptr), Promise<T>(state_ptr)};
}

}  // namespace exe::future
