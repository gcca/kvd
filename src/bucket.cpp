#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>

#include "bucket.hpp"

using namespace kvd;

namespace {

class VariantBucket : public Bucket {
public:
  std::string &GetString(const std::string_view &key) noexcept final {
    return Accept(key, std::string{""});
  }

  std::size_t &GetCount(const std::string_view &key) noexcept final {
    return Accept(key, 0UL);
  }

  std::vector<std::string_view> Keys() const noexcept final {
    std::vector<std::string_view> keys;
    keys.reserve(map_.size());
    std::transform(map_.cbegin(), map_.cend(), std::back_inserter(keys),
                   [](const auto &p) { return std::string_view{p.first}; });
    return keys;
  }

  void Transact(std::function<void()> call) noexcept final {
    std::lock_guard<std::mutex> lock{mutex_};
    call();
  }

protected:
  template <class V>
  inline V &Accept(const std::string_view &key, V &&def) noexcept {
    const std::string skey{key};

    if (not map_.contains(skey)) { map_[skey] = std::forward<V>(def); }

    return *std::get_if<V>(&map_[skey]);
  }

private:
  std::mutex mutex_;
  std::unordered_map<std::string, std::variant<std::string, std::size_t>> map_;
};

} // namespace

std::unique_ptr<Bucket> Bucket::Make() {
  return std::make_unique<VariantBucket>();
}
