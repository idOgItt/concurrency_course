#pragma once

#include "body.hpp"
#include "coroutine.hpp"
#include "twist/ed/std/atomic.hpp"
#include "exe/runtime/view.hpp"

#include <memory>
#include <atomic>

namespace exe::fiber {

class Fiber : public std::enable_shared_from_this<Fiber> {
 public:
  Fiber(runtime::View, Body);

  void Resume();
  void Yield();
  void Park();

  runtime::View GetView() const {
    return view_;
  }
  static Fiber& Self();

 private:
  void Schedule();

 private:
  runtime::View view_;
  Coroutine coroutine_;
  twist::ed::std::atomic_bool scheduled_{false};
  bool auto_reschedule_{true};
};

using FiberPtr = std::shared_ptr<exe::fiber::Fiber>;

}  // namespace exe::fiber
