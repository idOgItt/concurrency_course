#pragma once

#include <exe/runtime/task/scheduler.hpp>

#include <utility>

namespace exe::runtime {

template <typename F>
void SubmitTask(task::IScheduler& scheduler, F fun) {
  scheduler.Submit(std::move(fun));
}

}  // namespace exe::runtime
