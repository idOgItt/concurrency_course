#include "suspend.hpp"
#include <exe/fiber/core/fiber.hpp>

namespace exe::fiber {

void Suspend(Body callback) {
  Fiber::Self().Suspend(std::move(callback));
}

}  // namespace exe::fiber
