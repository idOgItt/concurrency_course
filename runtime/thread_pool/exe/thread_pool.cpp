#include "thread_pool.hpp"
#include "queue.hpp"
#include "task.hpp"

#include <twist/ed/std/thread.hpp>
#include <twist/ed/static/thread_local/ptr.hpp>
#include <wheels/core/panic.hpp>

#include <cassert>

namespace {

TWISTED_STATIC_THREAD_LOCAL_PTR(ThreadPool, thread_pool_ptr);

}  // namespace

ThreadPool::ThreadPool(size_t threads)
    : threads_(threads) {
}

ThreadPool::~ThreadPool() {
  assert(stopped_ && "ThreadPool must be stopped before destruction");
}

void ThreadPool::Start() {
  assert(!started_ && "ThreadPool::Start() should be called only once");
  started_ = true;

  workers_.reserve(threads_);
  for (size_t i = 0; i < threads_; ++i) {
    workers_.emplace_back([this] {
      WorkerRoutine();
    });
  }
}

void ThreadPool::Submit(Task task) {
  assert(started_ && "ThreadPool::Submit() called before Start()");
  tasks_.Push(std::move(task));
}

ThreadPool* ThreadPool::Current() {
  return thread_pool_ptr;
}

void ThreadPool::Stop() {
  assert(started_ && "ThreadPool::Stop() called before Start()");
  assert(!stopped_ && "ThreadPool::Stop() should be called only once");

  tasks_.Close();

  for (auto& worker : workers_) {
    worker.join();
  }

  stopped_ = true;
}

void ThreadPool::WorkerRoutine() {
  thread_pool_ptr = this;

  while (true) {
    auto task_opt = tasks_.Pop();
    if (!task_opt.has_value()) {
      break;
    }

    try {
      (*task_opt)();
    } catch (...) {
      wheels::Panic("Unhandled exception in ThreadPool worker");
    }
  }

  thread_pool_ptr = nullptr;
}
