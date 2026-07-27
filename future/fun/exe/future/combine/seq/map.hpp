#pragma once

#include <exe/future/syntax/pipe.hpp>
#include <exe/runtime/submit_task.hpp>

#include <type_traits>

namespace exe::future {

namespace pipe {

template <typename Fn>
struct [[nodiscard]] Map {
  Fn func;

  explicit Map(Fn f)
      : func(std::move(f)) {
  }

  Map(const Map&) = delete;

  template <typename T>
  using ResultType = std::invoke_result_t<Fn, T>;

  template <typename T>
  Future<ResultType<T>> Pipe(Future<T> input) {
    auto rt = input.Runtime();
    auto [fut, prom] = Contract<ResultType<T>>(rt);

    std::move(input).Consume(
        [rt, prom = std::move(prom), fn = std::move(func)](T val) mutable {
          runtime::SubmitTask(rt, [fn = std::move(fn), prom = std::move(prom),
                                   val = std::move(val)] mutable {
            std::move(prom).Set(fn(std::move(val)));
          });
        });

    return std::move(fut);
  }
};

}  // namespace pipe

template <typename Fn>
auto Map(Fn fn) {
  return pipe::Map{std::move(fn)};
}

}  // namespace exe::future
