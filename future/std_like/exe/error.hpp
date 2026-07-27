#pragma once

#include <stdexcept>

struct NoStateError : std::runtime_error {
  NoStateError()
      : std::runtime_error("No state available") {
  }
};

struct PromiseAlreadySatisfiedError : std::runtime_error {
  PromiseAlreadySatisfiedError()
      : std::runtime_error("Promise already satisfied") {
  }
};

struct BrokenPromiseError : std::runtime_error {
  BrokenPromiseError()
      : std::runtime_error("Broken promise") {
  }
};
