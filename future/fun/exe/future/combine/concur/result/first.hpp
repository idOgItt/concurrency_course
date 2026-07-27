#pragma once

#include <exe/future/type/result.hpp>

namespace exe::future {
namespace detail {

template <typename T, typename Err>
struct RaceOkState {
  Promise<Result<T, Err>> result_;
  twist::ed::std::atomic<size_t> fails_{0};
  twist::ed::std::atomic<bool> resolved_{false};

  explicit RaceOkState(Promise<Result<T, Err>> res)
      : result_(std::move(res)) {
  }

  void TrySet(Result<T, Err> res) {
    if (resolved_.load(std::memory_order_acquire)) {
      return;
    }

    if (res.has_value()) {
      bool expected = false;
      if (resolved_.compare_exchange_strong(expected, true)) {
        std::move(result_).Set(std::move(res));
      }
    } else {
      if (fails_.fetch_add(1) == 1) {
        bool expected = false;
        if (resolved_.compare_exchange_strong(expected, true)) {
          std::move(result_).Set(std::move(res));
        }
      }
    }
  }
};

}  // namespace detail

template <typename T, typename Err>
TryFuture<T, Err> FirstOk(TryFuture<T, Err> lhs, TryFuture<T, Err> rhs) {
  auto runtime = lhs.Runtime();
  auto [fut, prom] = Contract<Result<T, Err>>(runtime);

  auto shared = std::make_shared<detail::RaceOkState<T, Err>>(std::move(prom));

  auto handler = [shared](Result<T, Err> res) {
    shared->TrySet(std::move(res));
  };

  std::move(lhs).Consume(handler);
  std::move(rhs).Consume(handler);

  return std::move(fut);
}

}  // namespace exe::future
