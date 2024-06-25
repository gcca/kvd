#include <algorithm>
#include <iterator>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

#include "bucket.hpp"
#include "logical_plan.hpp"
#include "op.hpp"
#include "result.hpp"

using namespace kvd;

namespace {

class OpLogicalPlan : public LogicalPlan {
public:
  explicit OpLogicalPlan(std::vector<std::unique_ptr<Op>> &&ops)
      : ops_{std::move(ops)} {}

  void Explain(std::ostream &os) const final {
    std::copy(ops_.begin(), std::prev(ops_.end()),
              std::ostream_iterator<std::unique_ptr<Op>>(os, "\n"));
    os << ops_.back();
  }

  [[nodiscard]] std::vector<std::unique_ptr<Result>>
  Execute(Bucket &bucket) const noexcept final {
    std::vector<std::unique_ptr<Result>> results;
    results.reserve(ops_.size());
    bucket.Transact([&]() {
      std::transform(
          ops_.cbegin(), ops_.cend(), std::back_inserter(results),
          [&](const std::unique_ptr<Op> &op) { return op->Execute(bucket); });
    });
    return results;
  }

private:
  std::vector<std::unique_ptr<Op>> ops_;
};

} // namespace

std::unique_ptr<LogicalPlan>
LogicalPlan::Make(std::vector<std::unique_ptr<Op>> &&ops) {
  return std::make_unique<OpLogicalPlan>(std::move(ops));
}
