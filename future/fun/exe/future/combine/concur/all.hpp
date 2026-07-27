#pragma once

#include <exe/future/type/future.hpp>

#include <cstdlib>
#include <tuple>

namespace exe::future {

namespace detail {

template <typename T, typename U>
struct PairState {
  Promise<std::tuple<T, U>> result;
  std::optional<T> first;
  std::optional<U> second;
  twist::ed::std::atomic<size_t> count{0};

  explicit PairState(Promise<std::tuple<T, U>> r)
      : result(std::move(r)) {
  }

  void TrySet() {
    if (count.fetch_add(1) == 1) {
      std::move(result).Set({std::move(*first), std::move(*second)});
    }
  }
};

}  // namespace detail

template <typename T, typename U>
Future<std::tuple<T, U>> Both(Future<T> lhs, Future<U> rhs) {
  auto runtime = lhs.Runtime();
  auto [fut, prom] = Contract<std::tuple<T, U>>(runtime);
  auto shared = std::make_shared<detail::PairState<T, U>>(std::move(prom));

  std::move(lhs).Consume([shared](T val) {
    shared->first.emplace(std::move(val));
    shared->TrySet();
  });

  std::move(rhs).Consume([shared](U val) {
    shared->second.emplace(std::move(val));
    shared->TrySet();
  });

  return std::move(fut);
}

}  // namespace exe::future
