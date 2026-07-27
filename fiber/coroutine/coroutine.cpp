#include "coroutine.hpp"

namespace {
constexpr std::size_t kStackSize = 64 * 1024;
}

Coroutine::Coroutine(Body body)
    : body_(std::move(body)),
      stack_view_(
          twist::ed::sure::stack::GuardedMmapStack::AllocateAtLeastBytes(
              kStackSize)) {
  trampoline_ = std::make_unique<CoroutineTrampoline>(this);
  auto stack_view = stack_view_.MutView();
  execution_context_.Setup(stack_view, trampoline_.get());
}

void Coroutine::Resume() {
  if (done_) {  // NOLINT
    return;
  }
  caller_context_.SwitchTo(execution_context_);
}

void Coroutine::Suspend() {
  execution_context_.SwitchTo(caller_context_);
}

bool Coroutine::IsDone() const {
  return done_;
}
