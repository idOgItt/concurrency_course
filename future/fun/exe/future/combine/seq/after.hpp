#pragma once

#include <exe/future/syntax/pipe.hpp>

#include <cstdlib>
#include <chrono>
#include <exe/runtime/set_timer.hpp>

namespace exe::future {

namespace pipe {

struct [[nodiscard]] DelayPipe {
  std::chrono::microseconds duration;

  explicit DelayPipe(std::chrono::microseconds d)
      : duration(d) {
  }

  DelayPipe(const DelayPipe&) = delete;

  template <typename T>
  Future<T> Pipe(Future<T> input) {
    auto rt = input.Runtime();
    auto [fut, prom] = Contract<T>(rt);

    std::move(input).Consume(
        [d = std::move(duration), rt, prom = std::move(prom)](T val) mutable {
          runtime::SetTimer(
              rt, d, [prom = std::move(prom), val = std::move(val)] mutable {
                std::move(prom).Set(std::move(val));
              });
        });

    return std::move(fut);
  }
};

}  // namespace pipe

inline auto After(std::chrono::microseconds delay) {
  return pipe::DelayPipe{delay};
}

}  // namespace exe::future
