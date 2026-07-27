#include "runtime.hpp"

namespace exe::runtime::sandbox {

// Run

// Tasks

size_t Runtime::RunAtMostTasks(size_t limit) {
  size_t count = 0;
  while (count < limit && !tasks_.empty()) {
    auto task = std::move(tasks_.front());
    tasks_.pop();
    task();
    ++count;
  }
  return count;
}

size_t Runtime::RunTasks() {
  return RunAtMostTasks(std::numeric_limits<size_t>::max());
}

// Timers

size_t Runtime::AdvanceClockBy(timer::Duration delta) {
  clock_.AdvanceBy(delta);
  auto ready = timer_queue_.PopReady(clock_.Now());
  for (auto& handler : ready) {
    tasks_.push(std::move(handler));
  }
  return ready.size();
}

size_t Runtime::AdvanceClockToNextDeadline() {
  auto next_deadline = timer_queue_.NextDeadline();
  if (!next_deadline.has_value()) {
    return 0;
  }
  return AdvanceClockBy(next_deadline.value() - clock_.Now());
}
// Empty

bool Runtime::IsEmpty() const {
  return tasks_.empty() && timer_queue_.IsEmpty();
}

}  // namespace exe::runtime::sandbox
