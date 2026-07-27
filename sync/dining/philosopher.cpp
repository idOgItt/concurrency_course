#include "philosopher.hpp"

#include <twist/test/body/inject_fault.hpp>

namespace dining {

Philosopher::Philosopher(Table& table, size_t seat)
    : table_(table),
      seat_(seat),
      left_fork_(table_.LeftFork(seat)),
      right_fork_(table_.RightFork(seat)) {
}

void Philosopher::Eat() {
  AcquireForks();
  EatWithForks();
  ReleaseForks();
}

// Acquire left_fork_ and right_fork_
void Philosopher::AcquireForks() {
  while (true) {
    if (right_fork_.try_lock()) {
      if (left_fork_.try_lock()) {
        return;
      }
      right_fork_.unlock();
    }
    Think();
  }
}

void Philosopher::EatWithForks() {
  table_.AccessPlate(seat_);
  // Try to provoke data race
  table_.AccessPlate(table_.ToRight(seat_));
  ++meals_;
}

// Release left_fork_ and right_fork_
void Philosopher::ReleaseForks() {
  left_fork_.unlock();
  right_fork_.unlock();
}

void Philosopher::Think() {
  // Random pause or context switch
  twist::test::body::InjectFault();
}

}  // namespace dining
