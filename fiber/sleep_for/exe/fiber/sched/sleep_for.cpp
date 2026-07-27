#include "sleep_for.hpp"

#include "exe/fiber/core/fiber.hpp"
#include "exe/fiber/sched/yield.hpp"
#include "exe/runtime/set_timer.hpp"
#include "exe/runtime/submit_task.hpp"
#include "fmt/core.h"

namespace exe::fiber {

using std::chrono::operator""us;

void SleepFor(std::chrono::microseconds delay) {
  if (delay <= 0us) {
    Yield();
    return;
  }

  Fiber& self = Fiber::Self();
  auto view = self.GetView();
  auto self_sp = self.shared_from_this();

  exe::runtime::SetTimer(view, delay, [self_sp]() mutable {
    self_sp->Resume();
  });

  self.Park();
}

}  // namespace exe::fiber
