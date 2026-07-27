#pragma once

#include <exe/future/type/future.hpp>

namespace exe::future {
namespace detail {

template <typename T>
struct RaceState {
  Promise<T> result;
  twist::ed::std::atomic<bool> triggered{false};

  explicit RaceState(Promise<T> r)
      : result(std::move(r)) {
  }

  void TrySet(T val) {
    if (!triggered.exchange(true)) {
      std::move(result).Set(std::move(val));
    }
  }
};

}  // namespace detail

template <typename T>
Future<T> First(Future<T> lhs, Future<T> rhs) {
  auto runtime = lhs.Runtime();
  auto [fut, prom] = Contract<T>(runtime);
  auto shared = std::make_shared<detail::RaceState<T>>(std::move(prom));

  auto handler = [shared](T val) {
    shared->TrySet(std::move(val));
  };

  std::move(lhs).Consume(handler);
  std::move(rhs).Consume(handler);

  return std::move(fut);
}

}  // namespace exe::future
