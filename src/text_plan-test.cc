#include <memory>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "text_plan.hpp"

enum class TestPlanType { GET, PUT, COUNT, INC, VOID, FULL };

template <TestPlanType> struct TestPlanInfo;
#define TEST_PLAN_INFO(T, S) /* NOLINT */                                      \
  template <> struct TestPlanInfo<TestPlanType::T> {                           \
    static constexpr char name[] = #T;     /* NOLINT */                        \
    static constexpr char explained[] = S; /* NOLINT */                        \
  }
TEST_PLAN_INFO(GET, "GET:key");
TEST_PLAN_INFO(PUT, "PUT:key,value");
TEST_PLAN_INFO(COUNT, "COUNT:key");
TEST_PLAN_INFO(INC, "INC:key");
TEST_PLAN_INFO(VOID, "VOID");
TEST_PLAN_INFO(FULL, ("GET:key\n"
                      "PUT:key,value\n"
                      "COUNT:key\n"
                      "INC:key\n"
                      "VOID"));

template <std::unique_ptr<kvd::TextPlan> (*M)(const std::string &),
          TestPlanType>
struct TText;

template <std::unique_ptr<kvd::TextPlan> (*...M)(const std::string &)>
struct MakeList;

#define ZTEXT(T, S) /* NOLINT */                                               \
  template <>                                                                  \
  struct TText<kvd::TextPlan::MakeZ, TestPlanType::T>                          \
      : TestPlanInfo<TestPlanType::T> {                                        \
    static constexpr auto Make = &kvd::TextPlan::MakeZ;                        \
    static constexpr char make_name[] = "MakeZ"; /* NOLINT */                  \
    static constexpr char text[] = S;            /* NOLINT */                  \
  }
ZTEXT(GET, "\1\3GET\1\1\3key");
ZTEXT(PUT, "\1\3PUT\2\1\3key\1\5value");
ZTEXT(COUNT, "\1\5COUNT\1\1\3key");
ZTEXT(INC, "\1\3INC\1\1\3key");
ZTEXT(VOID, "\1\4VOID");
ZTEXT(FULL, ("\5\3GET\1\1\3key"
             "\3PUT\2\1\3key\1\5value"
             "\5COUNT\1\1\3key"
             "\3INC\1\1\3key"
             "\4VOID"));

#define YTEXT(T, S) /* NOLINT */                                               \
  template <>                                                                  \
  struct TText<kvd::TextPlan::MakeY, TestPlanType::T>                          \
      : TestPlanInfo<TestPlanType::T> {                                        \
    static constexpr auto Make = &kvd::TextPlan::MakeY;                        \
    static constexpr char make_name[] = "MakeY"; /* NOLINT */                  \
    static constexpr char text[] = S;            /* NOLINT */                  \
  }
YTEXT(GET, "GET key");
YTEXT(PUT, "PUT key value");
YTEXT(COUNT, "COUNT key");
YTEXT(INC, "INC key");
YTEXT(VOID, "VOID");
YTEXT(FULL, ("GET key\n"
             "PUT key value\n"
             "COUNT key\n"
             "INC key\n"
             "VOID"));

template <class> struct PlanTestTypes;
template <TestPlanType...> struct TypeList;
template <TestPlanType... T> struct PlanTestTypes<TypeList<T...>> {
  using TestTypes = testing::Types<TText<kvd::TextPlan::MakeZ, T>...,
                                   TText<kvd::TextPlan::MakeY, T>...>;
};

using Makes = MakeList<kvd::TextPlan::MakeZ, kvd::TextPlan::MakeY>;
using Types =
    TypeList<TestPlanType::GET, TestPlanType::PUT, TestPlanType::COUNT,
             TestPlanType::INC, TestPlanType::VOID, TestPlanType::FULL>;

template <class T> class ExplainTextPlanTest : public testing::Test {};

struct ExplainTextPlanTestNamer {
  template <typename T> static std::string GetName(int /**/) {
    std::ostringstream oss;
    oss << T::make_name << '/' << T::name;
    return oss.str();
  }
};

using TextPlanTestTypes = PlanTestTypes<Types>::TestTypes;
TYPED_TEST_SUITE(ExplainTextPlanTest, TextPlanTestTypes,
                 ExplainTextPlanTestNamer);

TYPED_TEST(ExplainTextPlanTest, Text) {
  const std::string text{TypeParam::text};
  auto plan = TypeParam::Make(text);
  ASSERT_EQ(plan->Explain(), TypeParam::explained);
}

TYPED_TEST(ExplainTextPlanTest, Logical) {
  const std::string text{TypeParam::text};
  auto plan = TypeParam::Make(text);
  ASSERT_EQ(plan->ToLogicalPlan()->Explain(), TypeParam::explained);
}
