#pragma once

#include <memory>
#include <utility>

class UniqueFunction {
 private:
  struct IRunnable {
    virtual ~IRunnable() = default;
    virtual void Run() = 0;
  };

  template <typename F>
  struct RunnableImpl : IRunnable {
    F func;

    explicit RunnableImpl(F&& f)
        : func(std::move(f)) {
    }

    void Run() override {
      func();
    }
  };

  std::unique_ptr<IRunnable> ptr_;

 public:
  template <typename F>
  explicit UniqueFunction(F&& f)
      : ptr_(std::make_unique<RunnableImpl<std::decay_t<F>>>(
            std::forward<F>(f))) {
  }

  // Movable
  UniqueFunction(UniqueFunction&& other) noexcept = default;
  UniqueFunction& operator=(UniqueFunction&& other) noexcept = default;

  // Non-copyable
  UniqueFunction(const UniqueFunction&) = delete;
  UniqueFunction& operator=(const UniqueFunction&) = delete;

  void operator()() {
    if (ptr_) {
      ptr_->Run();
    }
  }
};
