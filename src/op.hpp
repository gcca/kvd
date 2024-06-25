#pragma once

#include <memory>
#include <ostream>

#include "core.hpp"
#include "result.hpp"

namespace kvd {

class Op {
  KVD_PROTO(Op);

  friend std::ostream &operator<<(std::ostream &os,
                                  const std::unique_ptr<Op> &op) {
    op->Explain(os);
    return os;
  }

public:
  virtual void Explain(std::ostream &) const noexcept = 0;

  [[nodiscard]] virtual std::unique_ptr<Result>
  Execute(class Bucket &) const = 0;

  [[nodiscard]] inline std::unique_ptr<Result>
  Execute(std::unique_ptr<Bucket> &bucket) const {
    return Execute(*bucket);
  }
};

} // namespace kvd
