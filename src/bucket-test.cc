#include <algorithm>

#include <gtest/gtest.h>
#include <iterator>

#include "bucket.hpp"
#include "test-utils.hpp"

class BucketTest : public testing::Test {
protected:
  void SetUp() override { bucket = kvd::Bucket::Make(); }

  std::unique_ptr<kvd::Bucket> bucket; // NOLINT
};

using BucketTypes = testing::Types<kvd::Bucket::String, kvd::Bucket::Count>;

template <class T> class TypedBucketTest : public BucketTest {};

template <class T> struct TestBucketTraits;

template <> struct TestBucketTraits<kvd::Bucket::String> {
  static constexpr std::string_view default_value;
  static constexpr std::string_view test_value = "value";
  static constexpr auto Get = &kvd::Bucket::GetString;
};

template <> struct TestBucketTraits<kvd::Bucket::Count> {
  static constexpr std::size_t default_value = 0;
  static constexpr std::size_t test_value = 12345;
  static constexpr auto Get = &kvd::Bucket::GetCount;
};

struct BucketTypesNames {
  template <typename T> static std::string GetName(int /**/) {
    if constexpr (std::is_same_v<T, kvd::Bucket::String>) {
      return "kvd::Bucket::String";
    }
    if constexpr (std::is_same_v<T, kvd::Bucket::Count>) {
      return "kvd::Bucket::Count";
    }
    return "Unknown";
  }
};

TYPED_TEST_SUITE(TypedBucketTest, BucketTypes, BucketTypesNames);

TYPED_TEST(TypedBucketTest, GetDefault) {
  using Traits = TestBucketTraits<TypeParam>;
  TypeParam &value = (this->bucket.get()->*Traits::Get)("Key");
  ASSERT_EQ(value, Traits::default_value);
}

TYPED_TEST(TypedBucketTest, GetUpdate) {
  using Traits = TestBucketTraits<TypeParam>;
  (this->bucket.get()->*Traits::Get)("Key") = Traits::test_value;
  ASSERT_EQ((this->bucket.get()->*Traits::Get)("Key"), Traits::test_value);
}

TEST(KeysBucketTest, List) {
  auto bucket = kvd::Bucket::Make();
  bucket->GetString("Key 1") = "";
  bucket->GetString("Key 2") = "";
  bucket->GetCount("Key 3") = 0;
  bucket->GetCount("Key 4") = 0;

  auto keys = bucket->Keys();

  ASSERT_EQ(keys.size(), 4);

  std::sort(keys.begin(), keys.end());

  ASSERT_EQ(
      keys, //
      (std::vector<std::string_view>{"Key 1", "Key 2", "Key 3", "Key 4"}));
}

TEST(AtomicBucketTest, IncThreaded) {
  auto bucket = kvd::Bucket::Make();

  const std::size_t incs_size = 1000;
  const std::size_t threads_size = 25;
  std::vector<std::vector<std::size_t>> threads_results;
  threads_results.resize(threads_size);
  {
    std::vector<std::jthread> threads;
    threads.reserve(threads_size);

    for (std::size_t i = 0; i < threads_size; i++) {
      threads.emplace_back(
          [&](const std::size_t i) {
            bucket->Transact([&]() {
              threads_results[i].resize(incs_size);
              std::generate(threads_results[i].begin(),
                            threads_results[i].end(),
                            [&]() { return ++bucket->GetCount("Key"); });
            });
          },
          i);
    }
  }

  const std::size_t results_size = threads_size * incs_size;

  std::vector<std::size_t> results;
  results.reserve(results_size);

  auto it = std::back_inserter(results);
  for (const auto &thread_results : threads_results) {
    results.insert(results.end(), thread_results.cbegin(),
                   thread_results.cend());
  }

  std::sort(results.begin(), results.end());

  std::vector<std::size_t> expected_results;
  expected_results.reserve(results_size);
  std::generate_n(std::back_inserter(expected_results), results_size,
                  [n = 0]() mutable { return ++n; });

  ASSERT_EQ(results, expected_results);
}
