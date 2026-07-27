#pragma once

#include <exe/future/type/future.hpp>

#include <utility>

namespace exe::future {

template <typename T>
Future<T> Ready(T val) {
  auto [fut, prom] = Contract<T>();
  std::move(prom).Set(std::move(val));
  return std::move(fut);
}

}  // namespace exe::future
