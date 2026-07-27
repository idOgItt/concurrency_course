#pragma once

#include <exe/future/type/result.hpp>

#include <cstdlib>
#include <utility>
#include "exe/future/make/result/ok.hpp"

namespace exe::future {

namespace pipe {

template <typename Fn>
struct [[nodiscard]] OrElse {
  Fn handler;

  explicit OrElse(Fn f)
      : handler(std::move(f)) {
  }

  OrElse(const OrElse&) = delete;

  template <typename T, typename E>
  TryFuture<T, E> Pipe(TryFuture<T, E> input) {
    return std::move(input) |
           FlatMap([handler = std::move(handler)](Result<T, E> res) mutable {
             if (res.has_value()) {
               return Ok<T, E>(std::move(*res));
             } else {
               return handler(std::move(res.error()));
             }
           });
  }
};

}  // namespace pipe

template <typename Fn>
auto OrElse(Fn fn) {
  return pipe::OrElse{std::move(fn)};
}

}  // namespace exe::future
