#include "thread_pool.hpp"

namespace exe::runtime {

TWISTED_STATIC_THREAD_LOCAL_PTR(ThreadPool, this_thread_pool);

ThreadPool::ThreadPool(size_t threads)
    : threads_count_(threads) {
  threads_.reserve(threads);
}

void ThreadPool::Start() {
  for (size_t i = 0; i < threads_count_; ++i) {
    auto join_handle = twist::ed::std::thread([this] {
      this_thread_pool = this;

      while (true) {
        auto current = queue_.Pop();

        if (current.has_value()) {
          (current.value())();
        } else {
          break;
        }
      }
    });

    my_thread_ids_.insert(join_handle.get_id());
    threads_.push_back(std::move(join_handle));
  }
}

ThreadPool::~ThreadPool() {
  assert(stopped_ == true);
}

bool ThreadPool::HasThread(twist::ed::std::thread::id id) const {
  return my_thread_ids_.contains(id);
}

void ThreadPool::Submit(task::Task task) {
  queue_.Push(std::move(task));
}

ThreadPool* ThreadPool::Current() {
  return this_thread_pool;
}

void ThreadPool::Stop() {
  queue_.Close();

  for (auto& thread : threads_) {
    thread.join();
  }

  stopped_ = true;
}

}  // namespace exe::runtime