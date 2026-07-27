#pragma once

#include "twist/ed/std/thread.hpp"
#include "sync/queue.hpp"
#include <exe/runtime/task/scheduler.hpp>

#include <cstddef>

namespace exe::runtime::multi_thread {

class ThreadPool final : public task::IScheduler {
 public:
  explicit ThreadPool(size_t num_threads);
  ~ThreadPool();

  // Non-copyable
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // Non-movable
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  void Start();

  // task::IScheduler
  void Submit(task::Task) override;

  static ThreadPool* Current();

  bool Here() const;

  void Stop();

 private:
  void WorkerRoutine();

 private:
  size_t threads_;
  bool started_{false};
  bool stopped_{false};

  std::vector<twist::ed::std::thread> workers_;
  UnboundedBlockingQueue<task::Task> tasks_;
};

}  // namespace exe::runtime::multi_thread
