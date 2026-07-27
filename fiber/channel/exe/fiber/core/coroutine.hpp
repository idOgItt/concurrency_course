#pragma once

#include "body.hpp"
#include "stack.hpp"

#include <twist/ed/sure/context.hpp>
#include <twist/ed/sure/stack/mmap.hpp>

#include <function2/function2.hpp>

namespace exe::fiber {

using Stack = twist::ed::sure::stack::GuardedMmapStack;

class Coroutine : private sure::ITrampoline {
 private:
  [[noreturn]] void Run() noexcept;

 public:
  explicit Coroutine(Body);

  void Resume();
  void Suspend();

  bool IsDone() const;

 private:
  Body body_;
  bool completed_{false};

  Stack stack_;
  twist::ed::sure::ExecutionContext body_context_;
  twist::ed::sure::ExecutionContext caller_context_;
};

}  // namespace exe::fiber
