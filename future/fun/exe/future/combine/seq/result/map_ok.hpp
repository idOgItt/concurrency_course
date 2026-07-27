#pragma once

#include <exe/future/type/result.hpp>

#include <type_traits>
#include <utility>
#include <exe/future/combine/seq/map.hpp>
#include <exe/result/combine/map.hpp>

namespace exe::future {

namespace pipe {

template <typename Fn>
struct [[nodiscard]] MapOk {
  Fn func;

  explicit MapOk(Fn f)
      : func(std::move(f)) {
  }

  MapOk(const MapOk&) = delete;

  template <typename T>
  using ResultType = std::invoke_result_t<Fn, T>;

  template <typename T, typename E>
  TryFuture<ResultType<T>, E> Pipe(TryFuture<T, E> input) {
    return std::move(input) |
           Map([fn = std::move(func)](Result<T, E> res) mutable {
             return std::move(res) | result::Map(std::move(fn));
           });
  }
};

}  // namespace pipe

template <typename Fn>
auto MapOk(Fn fn) {
  return pipe::MapOk(std::move(fn));
}

}  // namespace exe::future
