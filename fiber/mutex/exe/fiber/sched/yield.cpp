#include "yield.hpp"
#include <exe/fiber/core/fiber.hpp>

namespace exe::fiber {

void Yield() {
  Fiber::Self().Yield();
}

}  // namespace exe::fiber
