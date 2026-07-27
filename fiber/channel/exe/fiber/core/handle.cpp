#include "handle.hpp"

#include "fiber.hpp"

#include <cassert>
#include <utility>

namespace exe::fiber {

Fiber* FiberHandle::Release() {
  assert(IsValid());
  return std::exchange(fiber_, nullptr);
}

void FiberHandle::Resume() {
  assert(IsValid());

  Fiber* fiber = Release();
  fiber->Resume();
}

}  // namespace exe::fiber
