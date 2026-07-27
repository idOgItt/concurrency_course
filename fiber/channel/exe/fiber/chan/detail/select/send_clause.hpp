#pragma once

#include <exe/unit.hpp>

namespace exe::fiber {

namespace detail {

template <typename T>
struct SendClause {
  using ReturnType = Unit;

  T& value;
};

}  // namespace detail

}  // namespace exe::fiber
