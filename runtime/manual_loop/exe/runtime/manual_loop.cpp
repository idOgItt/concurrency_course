#include "manual_loop.hpp"

namespace exe::runtime {

void ManualLoop::Submit(task::Task task) {
  tasks_.push(std::move(task));
}

// Run tasks

size_t ManualLoop::RunAtMostTasks(size_t limit) {
  size_t executed = 0;
  while (executed < limit && NonEmpty()) {
    auto task = std::move(tasks_.front());
    tasks_.pop();
    task();
    ++executed;
  }
  return executed;
}

size_t ManualLoop::RunTasks() {
  size_t executed = 0;
  while (NonEmpty()) {
    executed += RunAtMostTasks(tasks_.size());
  }
  return executed;
}

bool ManualLoop::IsEmpty() const {
  return tasks_.empty();
}

}  // namespace exe::runtime
