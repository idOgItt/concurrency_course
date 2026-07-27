#pragma once

#include <twist/ed/std/atomic.hpp>

namespace exe::future::detail {

class RendezvousStateMachine {
 public:
  bool Produce() {
    uintptr_t prev_state = flag_.fetch_or(kProducer);
    return (prev_state & kConsumer) != 0;
  }

  bool Consume() {
    uintptr_t prev_state = flag_.fetch_or(kConsumer);
    return (prev_state & kProducer) != 0;
  }

 private:
  static const uintptr_t kProducer = 1;
  static const uintptr_t kConsumer = 2;

  twist::ed::std::atomic<uintptr_t> flag_{0};
};

}  // namespace exe::future::detail
