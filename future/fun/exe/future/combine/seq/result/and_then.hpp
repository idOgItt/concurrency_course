#pragma once

#include <exe/future/type/result.hpp>

#include <exe/result/trait/value_of.hpp>
#include <exe/future/trait/value_of.hpp>

#include <type_traits>
#include <utility>
#include "exe/future/make/result/err.hpp"

namespace exe::future {

namespace pipe {

template <typename Fn>
struct [[nodiscard]] AndThen {
  Fn func;

  explicit AndThen(Fn f)
      : func(std::move(f)) {
  }

  AndThen(const AndThen&) = delete;

  template <typename T>
  using ResultType = result::trait::ValueOf<
      future::trait::ValueOf<std::invoke_result_t<Fn, T>>>;

  template <typename T, typename E>
  TryFuture<ResultType<T>, E> Pipe(TryFuture<T, E> input) {
    return std::move(input) |
           FlatMap([fn = std::move(func)](Result<T, E> res) mutable {
             if (res.has_value()) {
               return fn(std::move(*res));
             } else {
               return Err<ResultType<T>, E>(std::move(res.error()));
             }
           });
  }
};

}  // namespace pipe

template <typename Fn>
auto AndThen(Fn fn) {
  return pipe::AndThen{std::move(fn)};
}

}  // namespace exe::future
