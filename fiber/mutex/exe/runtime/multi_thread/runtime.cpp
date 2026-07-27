#include "runtime.hpp"

namespace exe::runtime::multi_thread {

Runtime::Runtime(size_t num_workers)
    : thread_pool_(num_workers) {
}

void Runtime::Start() {
  thread_pool_.Start();
  if (use_timers_) {
    timer_thread_ = std::make_unique<multi_thread::TimerThread>(&thread_pool_);
    timer_thread_->Start();
  }
}

void Runtime::Stop() {
  if (timer_thread_) {
    timer_thread_->Stop();
    timer_thread_.reset();
  }
  thread_pool_.Stop();
}

bool Runtime::Here() const {
  return thread_pool_.Here();
}

Runtime::operator View() {
  return {&thread_pool_, timer_thread_.get()};
}

Runtime& Runtime::WithTimers() {
  use_timers_ = true;
  return *this;
}

}  // namespace exe::runtime::multi_thread
