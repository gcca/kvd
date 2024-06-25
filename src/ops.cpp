#include <memory>
#include <string>

#include "bucket.hpp"
#include "ops.hpp"
#include "result.hpp"

using namespace kvd;

namespace {

class StringRefResult : public Result {
public:
  explicit StringRefResult(const std::string &s) : s_{s} {}

  [[nodiscard]] const std::string &ToString() const noexcept final {
    return s_;
  }

private:
  const std::string &s_;
};

class StringResult : public Result {
public:
  explicit StringResult(const std::string &&s) : s_{s} {}

  [[nodiscard]] const std::string &ToString() const noexcept final {
    return s_;
  }

private:
  const std::string s_;
};

class CountResult : public Result {
public:
  explicit CountResult(const std::size_t s) : s_{std::to_string(s)} {}

  [[nodiscard]] const std::string &ToString() const noexcept final {
    return s_;
  }

private:
  const std::string s_;
};

const std::string D_OK{"$ok"}, D_PASS{"$pass"};

} // namespace

[[nodiscard]] std::unique_ptr<Result> GetOp::Execute(Bucket &bucket) const {
  return std::make_unique<StringRefResult>(bucket.GetString(std::string{key_}));
}

[[nodiscard]] std::unique_ptr<Result> PutOp::Execute(Bucket &bucket) const {
  bucket.GetString(std::string{key_}) = value_;
  return std::make_unique<StringRefResult>(D_OK);
}

[[nodiscard]] std::unique_ptr<Result> CountOp::Execute(Bucket &bucket) const {
  return std::make_unique<CountResult>(bucket.GetCount(std::string{key_}));
}

[[nodiscard]] std::unique_ptr<Result> IncOp::Execute(Bucket &bucket) const {
  return std::make_unique<CountResult>(++bucket.GetCount(std::string{key_}));
}

[[nodiscard]] std::unique_ptr<Result> VoidOp::Execute(Bucket & /**/) const {
  return std::make_unique<StringRefResult>(D_PASS);
}
