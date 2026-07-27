#pragma once

#include "exe/fiber/core/body.hpp"
#include "exe/fiber/core/handle.hpp"
namespace exe::fiber {

struct IAwaiter {
  virtual ~IAwaiter() = default;
  virtual void AwaitSuspend(FiberHandle) = 0;
};

void Suspend(Body);

}  // namespace exe::fiber
