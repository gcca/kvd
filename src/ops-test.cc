#include <memory>

#include <gtest/gtest.h>

#include "bucket.hpp"
#include "ops.hpp"

class OpsTest : public testing::Test {
protected:
  void SetUp() override { bucket = kvd::Bucket::Make(); }

  inline std::unique_ptr<kvd::Result> Do(kvd::Op &op) {
    return op.Execute(bucket);
  }

  std::unique_ptr<kvd::Bucket> bucket; // NOLINT
};

TEST_F(OpsTest, GetOpDefault) {
  kvd::GetOp op("Key");
  auto result = Do(op);
  ASSERT_EQ(result->ToString(), "");
}

TEST_F(OpsTest, GetOpUpdate) {
  bucket->GetString("Key") = "foo";

  kvd::GetOp op("Key");
  auto result = Do(op);
  ASSERT_EQ(result->ToString(), "foo");
}

TEST_F(OpsTest, PutOp) {
  kvd::PutOp op("Key", "Value");
  auto result = Do(op);
  ASSERT_EQ(result->ToString(), "$ok");
  ASSERT_EQ(bucket->GetString("Key"), "Value");
}

TEST_F(OpsTest, CountOpDefault) {
  kvd::CountOp op("Key");
  auto result = Do(op);
  ASSERT_EQ(result->ToString(), "0");
}

TEST_F(OpsTest, CountOpUpdate) {
  const std::size_t test_value = 12345;
  bucket->GetCount("Key") = test_value;

  kvd::CountOp op("Key");
  auto result = Do(op);
  ASSERT_EQ(result->ToString(), "12345");
}

TEST_F(OpsTest, IncOpDefault) {
  kvd::IncOp op("Key");
  auto result = Do(op);
  ASSERT_EQ(result->ToString(), "1");
}

TEST_F(OpsTest, IncOpUpdate) {
  const std::size_t test_value = 12345;
  bucket->GetCount("Key") = test_value;

  kvd::IncOp op("Key");
  auto result = Do(op);
  ASSERT_EQ(result->ToString(), "12346");
}

TEST_F(OpsTest, VoidOp) {
  kvd::VoidOp op;
  auto result = Do(op);
  ASSERT_EQ(result->ToString(), "$pass");
  ASSERT_EQ(bucket->Keys().size(), 0);
}
