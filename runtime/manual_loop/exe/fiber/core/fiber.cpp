#include "fiber.hpp"

#include <cassert>
#include <twist/ed/static/thread_local/ptr.hpp>

namespace {
TWISTED_STATIC_THREAD_LOCAL_PTR(exe::fiber::Fiber, tls_current_fiber);
}  // namespace

namespace exe::fiber {

Fiber::Fiber(Scheduler& sched, Body body)
    : scheduler_(&sched),
      coroutine_(
          [user_body = std::move(body)](Coroutine::SuspendContext) mutable {
            user_body();
          }) {
}

Fiber& Fiber::Self() {
  Fiber* current = tls_current_fiber;
  assert(current != nullptr && "Fiber::Self called outside of fiber context");
  return *current;
}

void Fiber::Schedule() {
  if (scheduled_) {
    return;
  }

  if (coroutine_.IsDone()) {
    return;
  }

  auto self_sp = shared_from_this();
  scheduler_->Submit([self_sp] {
    self_sp->Resume();
  });
}

void Fiber::Resume() {
  if (coroutine_.IsDone()) {
    return;
  }

  scheduled_ = false;

  assert(tls_current_fiber == nullptr &&
         "Resuming fiber on a thread that already has an active fiber");
  tls_current_fiber = this;

  coroutine_.Resume();

  tls_current_fiber = nullptr;

  if (!coroutine_.IsDone()) {
    Schedule();
  }
}

void Fiber::Yield() {
  coroutine_.Suspend();
}

}  // namespace exe::fiber