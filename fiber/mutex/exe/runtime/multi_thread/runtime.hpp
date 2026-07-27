#pragma once

#include <exe/runtime/view.hpp>

#include "exe/runtime/multi_thread/thread_pool.hpp"
#include "exe/runtime/multi_thread/timer_thread.hpp"

#include <optional>

namespace exe::runtime::multi_thread {

class Runtime {
 public:
  explicit Runtime(size_t num_workers);

  // NOLINTNEXTLINE
  operator View();

  Runtime& WithTimers();

  void Start();
  void Stop();

  bool Here() const;

 private:
  multi_thread::ThreadPool thread_pool_;
  std::unique_ptr<multi_thread::TimerThread> timer_thread_;
  bool use_timers_{false};
};

}  // namespace exe::runtime::multi_thread
