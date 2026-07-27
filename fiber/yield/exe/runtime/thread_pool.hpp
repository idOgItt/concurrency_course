#pragma once

#include "task.hpp"
#include "queue.hpp"

#include <cstddef>
#include <vector>
#include <twist/ed/std/thread.hpp>

namespace exe::runtime {

class ThreadPool {
 public:
  explicit ThreadPool(size_t threads);
  ~ThreadPool();

  // Non‑copyable
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // Non‑movable
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  void Start();
  void Submit(Task task);

  static ThreadPool* Current();

  void Stop();

 private:
  void WorkerRoutine();

 private:
  size_t threads_;
  bool started_{false};
  bool stopped_{false};

  std::vector<twist::ed::std::thread> workers_;
  UnboundedBlockingQueue<Task> tasks_;
};

}  // namespace exe::runtime