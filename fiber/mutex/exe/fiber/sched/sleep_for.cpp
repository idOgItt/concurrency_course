#include "sleep_for.hpp"
#include <exe/fiber/core/fiber.hpp>

namespace exe::fiber {

void SleepFor(std::chrono::microseconds delay) {
  Fiber::Self().SleepFor(delay);
}

}  // namespace exe::fiber
