#pragma once

#include <function2/function2.hpp>
#include <twist/ed/sure/stack/mmap.hpp>
#include <twist/ed/sure/context.hpp>

namespace exe::fiber {
class Coroutine {
 public:
  class SuspendContext {
    friend class Coroutine;

   public:
    void Suspend() const {
      self_->Suspend();
    }

   private:
    explicit SuspendContext(Coroutine* coro)
        : self_(coro) {
    }

   private:
    Coroutine* self_;
  };

 private:
  using Body = fu2::unique_function<void(SuspendContext)>;

 public:
  explicit Coroutine(Body);

  void Resume();
  void Suspend();

  bool IsDone() const;

 private:
  class CoroutineTrampoline : public sure::ITrampoline {
   public:
    explicit CoroutineTrampoline(Coroutine* coro)
        : coro_(coro) {
    }
    [[noreturn]] void Run() noexcept override {
      coro_->body_(SuspendContext(coro_));
      coro_->done_ = true;
      coro_->execution_context_.ExitTo(coro_->caller_context_);
      std::abort();  // NOLINT
    }
    virtual ~CoroutineTrampoline() = default;

   private:
    Coroutine* coro_;
  };

 private:
  Body body_;
  bool done_ = false;
  twist::ed::sure::ExecutionContext execution_context_;
  twist::ed::sure::ExecutionContext caller_context_;
  twist::ed::sure::stack::GuardedMmapStack stack_view_;
  std::unique_ptr<CoroutineTrampoline> trampoline_;
};
}  // namespace exe::fiber