#pragma once

#include "body.hpp"
#include "coroutine.hpp"

#include <exe/fiber/core/handle.hpp>
#include <twist/ed/std/atomic.hpp>
#include <exe/runtime/view.hpp>
#include <exe/runtime/task/task.hpp>
#include <twist/ed/std/mutex.hpp>

#include <chrono>

namespace exe::fiber {

class Fiber {
 private:
  Fiber(runtime::View view_context, Body entry_point);
  void SetTimer(std::chrono::microseconds timeout, Body job);
  void SubmitTask(Body job);

 public:
  static void Go(runtime::View view_context, Body entry_point);
  void Go(Body entry_point);

  static FiberHandle Handle();

  void SleepFor(std::chrono::microseconds timeout);
  void Yield();
  void Suspend(Body entry_point);
  void Resume();

  static Fiber& Self();

 private:
  class Handlers {
    friend class Fiber;
    static void ResumeCoroutine(Fiber* instance);
  };

 private:
  Coroutine fiber_coroutine_;
  runtime::View view_context_;
  Body resume_callback_;
};

}  // namespace exe::fiber
