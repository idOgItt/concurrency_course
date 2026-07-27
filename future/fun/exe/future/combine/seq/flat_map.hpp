#pragma once

#include <exe/future/type/future.hpp>

#include <exe/future/trait/value_of.hpp>

#include <type_traits>
#include <utility>
#include <exe/future/combine/seq/flatten.hpp>

namespace exe::future {

namespace pipe {

template <typename Fn>
struct [[nodiscard]] FlatMap {
  Fn func;

  explicit FlatMap(Fn f)
      : func(std::move(f)) {
  }

  FlatMap(const FlatMap&) = delete;

  template <typename T>
  using ResultType = future::trait::ValueOf<std::invoke_result_t<Fn, T>>;

  template <typename T>
  Future<ResultType<T>> Pipe(Future<T> input) {
    return std::move(input) | Map(std::move(func)) | Flatten();
  }
};

}  // namespace pipe

template <typename Fn>
auto FlatMap(Fn fn) {
  return pipe::FlatMap{std::move(fn)};
}

}  // namespace exe::future
