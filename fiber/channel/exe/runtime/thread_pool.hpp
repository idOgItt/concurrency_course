#pragma once

#include "queue.hpp"

#include <cassert>

#include <twist/ed/static/thread_local/ptr.hpp>
#include <twist/ed/std/thread.hpp>

#include <exe/runtime/task/scheduler.hpp>
#include <exe/runtime/queue.hpp>
#include <unordered_set>

namespace exe::runtime {

class ThreadPool : public task::IScheduler {
 public:
  explicit ThreadPool(size_t threads);
  ~ThreadPool();

  // Non-copyable
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // Non-movable
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  bool HasThread(twist::ed::std::thread::id) const;

  void Start();

  void Submit(task::Task) override;

  static ThreadPool* Current();

  void Stop();

 private:
  std::vector<twist::ed::std::thread> threads_;
  size_t threads_count_;
  UnboundedBlockingQueue<task::Task> queue_;
  bool stopped_{false};
  std::unordered_set<twist::ed::std::thread::id> my_thread_ids_;
};

}  // namespace exe::runtime
