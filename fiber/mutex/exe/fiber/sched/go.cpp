#include "go.hpp"

#include <exe/fiber/core/fiber.hpp>

namespace exe::fiber {

void Go(runtime::View scheduler, Body body) {
  Fiber::Go(scheduler, std::move(body));
}

void Go(Body body) {
  Fiber::Self().Go(std::move(body));
}

}  // namespace exe::fiber
