#pragma once

#include <exe/future/syntax/pipe.hpp>

#include <utility>

namespace exe::future {

template <typename T>
void Detach(Future<T> fut) {
  std::move(fut).Consume([](T) {});
}

namespace pipe {

struct [[nodiscard]] Detach {
  template <typename T>
  void Pipe(Future<T> fut) {
    future::Detach(std::move(fut));
  }
};

}  // namespace pipe

inline auto Detach() {
  return pipe::Detach{};
}

}  // namespace exe::future
