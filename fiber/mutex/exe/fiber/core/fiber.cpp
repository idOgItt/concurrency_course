#include "fiber.hpp"

#include <chrono>
#include <cstdlib>
#include <twist/ed/static/thread_local/ptr.hpp>
#include <exe/runtime/view/timers.hpp>
#include <exe/runtime/view/tasks.hpp>

namespace exe::fiber {

TWISTED_STATIC_THREAD_LOCAL_PTR(Fiber, current_fiber_ptr);

Fiber::Fiber(runtime::View view_context, Body entry_point)
    : fiber_coroutine_(std::move(entry_point)),
      view_context_(view_context) {
}

void Fiber::Go(runtime::View view_context, Body entry_point) {
  auto fiber_obj = new Fiber(view_context, std::move(entry_point));
  fiber_obj->Resume();
}

void Fiber::Go(Body entry_point) {
  Go(view_context_, std::move(entry_point));
}

void Fiber::SubmitTask(Body job) {
  runtime::Tasks(view_context_).Submit(std::move(job));
}

void Fiber::SetTimer(std::chrono::microseconds timeout, Body job) {
  runtime::Timers(view_context_).Set(timeout, std::move(job));
}

void Fiber::Resume() {
  SubmitTask([this] {
    Handlers::ResumeCoroutine(this);
  });
}

void Fiber::Suspend(Body entry_point) {
  resume_callback_ = std::move(entry_point);
  fiber_coroutine_.Suspend();
}

void Fiber::Yield() {
  resume_callback_ = [this] {
    Resume();
  };
  fiber_coroutine_.Suspend();
}

void Fiber::SleepFor(std::chrono::microseconds timeout) {
  resume_callback_ = [timeout, this] {
    SetTimer(timeout, [this] {
      Handlers::ResumeCoroutine(this);
    });
  };
  fiber_coroutine_.Suspend();
}

Fiber& Fiber::Self() {
  assert(current_fiber_ptr != nullptr);
  return *current_fiber_ptr;
}

FiberHandle Fiber::Handle() {
  return FiberHandle(&Self());
}

void Fiber::Handlers::ResumeCoroutine(Fiber* instance) {
  current_fiber_ptr = instance;
  instance->fiber_coroutine_.Resume();
  if (!instance->fiber_coroutine_.IsDone()) {
    instance->resume_callback_();
  } else {
    delete instance;
  }
}

}  // namespace exe::fiber
