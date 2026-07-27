#pragma once

#include <twist/ed/std/atomic.hpp>

namespace exe::future {

namespace detail {

class SyncFlag {
 public:
  bool SetProducer() {
    uintptr_t prev = flags_.fetch_or(kProducerSet);
    return (prev & kConsumerSet) != 0;
  }

  bool SetConsumer() {
    uintptr_t prev = flags_.fetch_or(kConsumerSet);
    return (prev & kProducerSet) != 0;
  }

 private:
  static const uintptr_t kProducerSet = 1;
  static const uintptr_t kConsumerSet = 2;

  twist::ed::std::atomic<uintptr_t> flags_{0};
};

}  // namespace detail

}  // namespace exe::future
