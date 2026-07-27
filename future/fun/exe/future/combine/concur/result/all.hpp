#pragma once

#include <exe/future/type/result.hpp>
#include <exe/thread/spinlock.hpp>

#include <tuple>
#include "exe/result/make/err.hpp"
#include "exe/result/make/ok.hpp"

namespace exe::future {
namespace detail {

template <typename T1, typename T2, typename Err>
struct PairResultState {
  using Tuple = std::tuple<T1, T2>;

  Promise<Result<Tuple, Err>> result_;
  twist::ed::std::atomic<bool> ready_{false};

  thread::SpinLock lock_;
  std::optional<T1> left_;
  std::optional<T2> right_;

  explicit PairResultState(Promise<Result<Tuple, Err>> res)
      : result_(std::move(res)) {
  }

  void Fail(Err err) {
    bool expected = false;
    if (ready_.compare_exchange_strong(expected, true)) {
      std::move(result_).Set(result::Err<Tuple, Err>(std::move(err)));
    }
  }

  void SetLeft(T1 val) {
    thread::LockGuard guard(lock_);
    if (ready_.load()) {
      return;
    }
    left_.emplace(std::move(val));
    if (right_.has_value()) {
      ready_.store(true);
      std::move(result_).Set(
          result::Ok<Tuple, Err>({std::move(*left_), std::move(*right_)}));
    }
  }

  void SetRight(T2 val) {
    thread::LockGuard guard(lock_);
    if (ready_.load()) {
      return;
    }
    right_.emplace(std::move(val));
    if (left_.has_value()) {
      ready_.store(true);
      std::move(result_).Set(
          result::Ok<Tuple, Err>({std::move(*left_), std::move(*right_)}));
    }
  }
};

}  // namespace detail

template <typename T1, typename T2, typename Err>
TryFuture<std::tuple<T1, T2>, Err> BothOk(TryFuture<T1, Err> fut1,
                                          TryFuture<T2, Err> fut2) {
  auto runtime = fut1.Runtime();
  using Tuple = std::tuple<T1, T2>;
  auto [fut_out, prom_out] = Contract<Result<Tuple, Err>>(runtime);

  auto shared = std::make_shared<detail::PairResultState<T1, T2, Err>>(
      std::move(prom_out));

  std::move(fut1).Consume([shared](Result<T1, Err> res) {
    if (res.has_value()) {
      shared->SetLeft(std::move(*res));
    } else {
      shared->Fail(std::move(res.error()));
    }
  });

  std::move(fut2).Consume([shared](Result<T2, Err> res) {
    if (res.has_value()) {
      shared->SetRight(std::move(*res));
    } else {
      shared->Fail(std::move(res.error()));
    }
  });

  return std::move(fut_out);
}

}  // namespace exe::future
