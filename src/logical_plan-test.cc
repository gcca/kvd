#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "bucket.hpp"
#include "logical_plan.hpp"
#include "ops.hpp"
#include "result.hpp"

TEST(LogicalPlanTest, Explain) {
  std::vector<std::unique_ptr<kvd::Op>> ops;
  ops.emplace_back(std::make_unique<kvd::GetOp>("Key"));
  ops.emplace_back(std::make_unique<kvd::PutOp>("Key", "Value"));
  ops.emplace_back(std::make_unique<kvd::CountOp>("Key"));
  ops.emplace_back(std::make_unique<kvd::IncOp>("Key"));
  ops.emplace_back(std::make_unique<kvd::VoidOp>());

  auto logical_plan = kvd::LogicalPlan::Make(std::move(ops));

  ASSERT_EQ(logical_plan->Explain(), ("GET:Key\n"
                                      "PUT:Key,Value\n"
                                      "COUNT:Key\n"
                                      "INC:Key\n"
                                      "VOID"));
}

TEST(LogicalPlanTest, Execute) {
  std::vector<std::unique_ptr<kvd::Op>> ops;
  ops.emplace_back(std::make_unique<kvd::PutOp>("Key", "Value"));
  ops.emplace_back(std::make_unique<kvd::GetOp>("Key"));
  ops.emplace_back(std::make_unique<kvd::CountOp>("KCount"));
  ops.emplace_back(std::make_unique<kvd::IncOp>("KCount"));
  ops.emplace_back(std::make_unique<kvd::VoidOp>());

  auto logical_plan = kvd::LogicalPlan::Make(std::move(ops));

  auto bucket = kvd::Bucket::Make();
  auto results = logical_plan->Execute(*bucket);

  std::vector<std::string> outputs;
  outputs.reserve(ops.size());

  std::transform(results.cbegin(), results.cend(), std::back_inserter(outputs),
                 [](const std::unique_ptr<kvd::Result> &result) {
                   return result->ToString();
                 });

  ASSERT_EQ(outputs.size(), 5);

  ASSERT_EQ(outputs[0], "$ok");
  ASSERT_EQ(outputs[1], "Value");
  ASSERT_EQ(outputs[2], "0");
  ASSERT_EQ(outputs[3], "1");
  ASSERT_EQ(outputs[4], "$pass");
}
