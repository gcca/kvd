#pragma once

#include <memory>

#include "plan.hpp"
#include "result.hpp"

namespace kvd {

class LogicalPlan : public Plan {
  KVD_PROTO(LogicalPlan);

public:
  [[nodiscard]] virtual std::vector<std::unique_ptr<Result>>
  Execute(class Bucket &) const noexcept = 0;

  [[nodiscard]] static std::unique_ptr<LogicalPlan>
  Make(std::vector<std::unique_ptr<class Op>> &&);
};

} // namespace kvd
