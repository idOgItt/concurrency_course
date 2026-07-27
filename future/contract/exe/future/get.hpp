#pragma once

#include "future.hpp"
#include <exe/thread/event.hpp>
#include <optional>

namespace exe::future {

template <typename T>
T Get(Future<T> future) {
  thread::Event event;
  std::optional<T> result;

  std::move(future).Consume([&](T value) {
    result.emplace(std::move(value));
    event.Fire();
  });

  event.Wait();

  return std::move(*result);
}

}  // namespace exe::future
