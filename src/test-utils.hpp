#pragma once

#include <thread>
#include <utility>

namespace std {

class jthread { // NOLINT
public:
  template <class T, class... A>
  explicit jthread(T &&t, A &&...a)
      : t_{std::forward<T>(t), std::forward<A>(a)...} {}

  jthread(const jthread &) = delete;
  jthread &operator=(const jthread &) = delete;

  jthread(jthread &&j) noexcept : t_{std::move(j.t_)} {}
  jthread &operator=(jthread &&j) noexcept {
    t_ = std::move(j.t_);
    return *this;
    ;
  }
  ~jthread() {
    if (t_.joinable()) { t_.join(); }
  }

private:
  std::thread t_;
};

} // namespace std
