#pragma once

#include <iostream>
#include <type_traits>
#include <utility>

namespace kvd::pooling {

template <class Tp> class Poolable {
public:
  template <class... Args> void Reset(Args &&...args) {
    this->Tp::Reset(std::forward<Args>(args)...);
  }
};

template <class Tp>
  requires std::is_base_of_v<Poolable<Tp>, Tp>
class Pool {
public:
  class Pooled {
  public:
    inline explicit Pooled(Tp *t, Pool *pool) : t_{t}, pool_{pool} {}
    inline ~Pooled() { pool_->Dispose(t_); }
    inline Tp *operator->() const noexcept { return t_; }
    inline Tp &operator*() const noexcept { return *t_; }

    Pooled(const Pooled &) = delete;
    Pooled(const Pooled &&) = delete;
    Pooled &operator=(const Pooled &) = delete;
    Pooled &operator=(const Pooled &&) = delete;

  private:
    Tp *t_;
    Pool *pool_;
  };

  inline explicit Pool(const std::size_t init_size) {
    pool_.reserve(init_size);
  }

  ~Pool() {
    for (const Tp *t : pool_) { delete t; }
  }

  template <class... Args> [[nodiscard]] inline auto Acquire(Args &&...args) {
    if (pool_.empty()) {
      return Pooled(new Tp(std::forward<Args>(args)...), this);
    }
    Tp *tp = pool_.back();
    pool_.pop_back();
    return Pooled(tp, this);
  }

  [[nodiscard]] inline std::size_t Size() const noexcept {
    return pool_.size();
  }

  Pool(const Pool &) = delete;
  Pool(const Pool &&) = delete;
  Pool &operator=(const Pool &) = delete;
  Pool &operator=(const Pool &&) = delete;

private:
  std::vector<Tp *> pool_;

  void Dispose(Tp *t) { pool_.push_back(t); }
};

} // namespace kvd::pooling
