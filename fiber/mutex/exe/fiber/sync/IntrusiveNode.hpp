#pragma once

#include "twist/ed/std/atomic.hpp"

namespace exe::sync {
struct IntrusiveNode {
  twist::ed::std::atomic<IntrusiveNode*> next{nullptr};
};
}  // namespace exe::sync
