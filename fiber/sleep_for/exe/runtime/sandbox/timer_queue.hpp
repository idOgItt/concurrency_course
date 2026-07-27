#pragma once

#include "clock.hpp"

#include <map>
#include <exe/runtime/timer/handler.hpp>

#include <optional>

namespace exe::runtime::sandbox {

class TimerQueue {
 public:
  void Push(Instant deadline, timer::Handler handler) {
    timers_.emplace(deadline, std::move(handler));
  }

  bool IsEmpty() const {
    return timers_.empty();
  }

  std::optional<Instant> NextDeadline() const {
    if (timers_.empty()) {
      return std::nullopt;
    }
    return timers_.begin()->first;
  }

  std::vector<timer::Handler> PopReady(Instant now) {
    std::vector<timer::Handler> ready;
    auto end = timers_.end();
    for (auto it = timers_.begin(); it != end && it->first <= now;
         it = timers_.erase(it)) {
      ready.push_back(std::move(it->second));
    }
    return ready;
  }

 private:
  std::multimap<Instant, timer::Handler> timers_;
};

}  // namespace exe::runtime::sandbox
