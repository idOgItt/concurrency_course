#include "timer_thread.hpp"

namespace exe::runtime::multi_thread {

TimerThread::TimerThread(task::IScheduler* scheduler)
    : scheduler_(scheduler) {
}

TimerThread::~TimerThread() {
  Stop();
}

void TimerThread::Set(timer::Duration delay, timer::Handler handler) {
  auto deadline = Clock::now() + delay;
  timer_queue_.Push(deadline, std::move(handler));
  condition_.notify_one();
}

void TimerThread::Start() {
  worker_ = twist::ed::std::thread([this] {
    WorkerRoutine();
  });
}

void TimerThread::Stop() {
  {
    twist::ed::std::lock_guard lock(mutex_);
    stopped_ = true;
    condition_.notify_one();
  }
  if (worker_.joinable()) {
    worker_.join();
  }
}

void TimerThread::WorkerRoutine() {
  using namespace std::chrono_literals;
  static constexpr timer::Duration kPollPeriod{50us};

  twist::ed::std::unique_lock lock(mutex_);

  while (!stopped_ || !heap_.empty()) {
    FetchNewTimers(lock);

    if (!heap_.empty()) {
      auto next_deadline = heap_.front().deadline_;

      if (next_deadline <= Clock::now()) {
        ExecuteReadyTimer(lock);
        continue;
      }

      auto wake_at = std::min(next_deadline, Clock::now() + kPollPeriod);
      condition_.wait_until(lock, wake_at);
    } else {
      condition_.wait_until(lock, Clock::now() + kPollPeriod);
    }
  }
}

void TimerThread::FetchNewTimers(
    twist::ed::std::unique_lock<twist::ed::std::mutex>& lock) {
  lock.unlock();
  auto new_timers = timer_queue_.PopAll();
  lock.lock();

  for (auto& [deadline, handler] : new_timers) {
    heap_.push_back({deadline, std::move(handler)});
  }
  std::make_heap(heap_.begin(), heap_.end(), std::greater<>());
}

void TimerThread::ExecuteReadyTimer(
    twist::ed::std::unique_lock<twist::ed::std::mutex>& lock) {
  std::pop_heap(heap_.begin(), heap_.end(), std::greater<>());
  auto timer = std::move(heap_.back());
  heap_.pop_back();
  lock.unlock();

  scheduler_->Submit(std::move(timer.handler_));

  lock.lock();
}

void TimerThread::WaitForNewTimers(
    twist::ed::std::unique_lock<twist::ed::std::mutex>& lock) {
  while (true) {
    if (!timer_queue_.IsEmpty()) {
      break;
    }

    if (!heap_.empty()) {
      auto next_deadline = heap_.front().deadline_;
      if (Clock::now() >= next_deadline) {
        break;
      }

      if (condition_.wait_until(lock, next_deadline) ==
          std::cv_status::timeout) {
        break;
      }
    } else {
      condition_.wait(lock, [this] {
        return stopped_ || !timer_queue_.IsEmpty();
      });

      if (stopped_ || !timer_queue_.IsEmpty()) {
        break;
      }
    }
  }
}

}  // namespace exe::runtime::multi_thread
