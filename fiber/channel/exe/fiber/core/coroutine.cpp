#include "coroutine.hpp"

namespace exe::fiber {
Coroutine::Coroutine(Body body)
    : body_(std::move(body)),
      stack_(Stack::AllocateAtLeastBytes(1 * 1024 * 1024)) {
  body_context_.Setup(std::move(stack_.MutView()), this);
}

void Coroutine::Resume() {
  caller_context_.SwitchTo(body_context_);
}

void Coroutine::Suspend() {
  body_context_.SwitchTo(caller_context_);
}

bool Coroutine::IsDone() const {
  return completed_;
}

[[noreturn]] void Coroutine::Run() noexcept {
  body_();

  completed_ = true;
  body_context_.ExitTo(caller_context_);
};
}  // namespace exe::fiber