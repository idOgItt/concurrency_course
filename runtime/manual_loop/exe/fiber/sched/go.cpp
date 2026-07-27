#include "go.hpp"

#include "exe/fiber/core/fiber.hpp"

#include <cassert>

namespace exe::fiber {

void Go(Scheduler& sched, Body body) {
  auto fb = std::make_shared<Fiber>(sched, std::move(body));
  sched.Submit([fb] {
    fb->Resume();
  });
}

void Go(Body body) {
  Fiber& self = Fiber::Self();
  Scheduler* sched = self.GetScheduler();
  assert(sched != nullptr && "Go(body) called outside fiber context");
  Go(*sched, std::move(body));
}

}  // namespace exe::fiber
