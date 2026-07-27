#pragma once

#include <exe/future/syntax/pipe.hpp>
#include <exe/thread/event.hpp>

namespace exe::future {

template <typename T>
T Get(Future<T> fut) {
  thread::Event signal;
  std::optional<T> output;

  std::move(fut).Consume([&](T val) {
    output.emplace(std::move(val));
    signal.Fire();
  });

  signal.Wait();

  assert(output.has_value());
  return std::move(*output);
}

namespace pipe {

struct [[nodiscard]] Get {
  template <typename T>
  T Pipe(Future<T> fut) {
    return future::Get(std::move(fut));
  }
};

}  // namespace pipe

inline auto Get() {
  return pipe::Get{};
}

}  // namespace exe::future
