#include <string>

#include <gtest/gtest.h>

#include "pooling.hpp"

class ObjTest : public kvd::pooling::Poolable<ObjTest> {
public:
  explicit ObjTest(int a, const std::string b) : a_{a}, b_{std::move(b)} {}
  explicit ObjTest() : a_{0} {}

  void Reset(int a, const std::string b) {
    a_ = a;
    b_ = std::move(b);
  }

  [[nodiscard]] int a() const { return a_; }
  [[nodiscard]] std::string b() { return b_; }

private:
  int a_;
  std::string b_;
};

TEST(PoolTest, Usage) {
  auto pool = kvd::pooling::Pool<ObjTest>(2);
  {
    auto obj = pool.Acquire(1, "1");
    ASSERT_EQ(obj->a(), 1);
    ASSERT_EQ(obj->b(), "1");
    ASSERT_EQ(pool.Size(), 0);
  }
  ASSERT_EQ(pool.Size(), 1);
}
