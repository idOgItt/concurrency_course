#pragma once

#include <exe/runtime/view.hpp>
#include <exe/future/syntax/pipe.hpp>

#include <cstdlib>

namespace exe::future {

namespace pipe {

struct [[nodiscard]] WithRuntime {
  runtime::View rt;

  explicit WithRuntime(runtime::View view)
      : rt(view) {
  }

  WithRuntime(const WithRuntime&) = delete;

  template <typename T>
  Future<T> Pipe(Future<T> input) {
    auto [fut, prom] = Contract<T>(rt);

    std::move(input).Consume([prom = std::move(prom)](T val) mutable {
      std::move(prom).Set(std::move(val));
    });

    return std::move(fut);
  }
};

}  // namespace pipe

inline auto Via(runtime::View rt) {
  return pipe::WithRuntime{rt};
}

}  // namespace exe::future
