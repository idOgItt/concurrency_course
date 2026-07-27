#pragma once

#include <exe/future/syntax/pipe.hpp>

#include <exe/future/terminate/get.hpp>
#include <exe/runtime/submit_task.hpp>

namespace exe::future {

namespace pipe {

struct [[nodiscard]] Join {
  Join() = default;

  Join(const Join&) = delete;

  template <typename T>
  Future<T> Pipe(Future<Future<T>> nested) {
    auto rt = nested.Runtime();
    auto [fut, prom] = Contract<T>(rt);

    std::move(nested).Consume(
        [prom = std::move(prom)](Future<T> inner) mutable {
          std::move(inner).Consume([prom = std::move(prom)](T val) mutable {
            std::move(prom).Set(std::move(val));
          });
        });

    return std::move(fut);
  }
};

}  // namespace pipe

inline auto Flatten() {
  return pipe::Join{};
}

}  // namespace exe::future
