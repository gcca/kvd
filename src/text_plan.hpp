#pragma once

#include <memory>

#include "logical_plan.hpp"

namespace kvd {

class TextPlan : public Plan {
  KVD_PROTO(TextPlan);

public:
  [[nodiscard]] virtual std::unique_ptr<LogicalPlan> ToLogicalPlan() const = 0;

  [[nodiscard]] static std::unique_ptr<TextPlan> MakeZ(const std::string &);

  [[nodiscard]] static std::unique_ptr<TextPlan> MakeY(const std::string &);
};

} // namespace kvd
