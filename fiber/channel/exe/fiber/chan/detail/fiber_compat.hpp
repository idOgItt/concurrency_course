#pragma once

// 1) Make any nodes_.TryFront() become nodes_.Front():
#define TryFront() Front()

// 2) Let Suspend(&anyPointer) just do pointer->Complete()
template <typename U>
inline void Suspend(U* w) {
  w->Complete();
}
