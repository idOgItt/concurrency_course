#pragma once

#include <exe/future/type/future.hpp>
#include <exe/runtime/view.hpp>

#include <type_traits>
#include <exe/runtime/submit_task.hpp>

namespace exe::future {

template <typename Fn>
Future<std::invoke_result_t<Fn>> Spawn(runtime::View rt, Fn task) {
  auto [fut, prom] = Contract<std::invoke_result_t<Fn>>();

  runtime::SubmitTask(rt, [prom = std::move(prom), task = task] mutable {
    std::move(prom).Set(task());
  });

  return std::move(fut);
}

}  // namespace exe::future
