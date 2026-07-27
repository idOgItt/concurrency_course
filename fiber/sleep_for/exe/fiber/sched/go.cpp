#include "go.hpp"

#include "exe/fiber/core/fiber.hpp"
#include "exe/runtime/submit_task.hpp"
#include <cassert>

namespace exe::fiber {

void Go(runtime::View view, Body body) {
  auto fb = std::make_shared<Fiber>(view, std::move(body));
  runtime::SubmitTask(view, [fb] {
    fb->Resume();
  });
}

void Go(Body body) {
  Fiber& self = Fiber::Self();
  runtime::View view = self.GetView();
  Go(view, std::move(body));
}

}  // namespace exe::fiber
