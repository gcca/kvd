#pragma once

#include <functional>
#include <memory>
#include <string_view>
#include <vector>

#include "bucket_utils.hpp"
#include "core.hpp"

#define BUCKET_GET(T, M)                                                       \
  template <> inline T &Get(const std::string_view &k) noexcept { return M(k); }

namespace kvd {
class Bucket {
  KVD_PROTO(Bucket);

public:
  using String = std::string;
  using Count = std::size_t;

  [[nodiscard]] virtual String &
  GetString(const std::string_view &) noexcept = 0;

  [[nodiscard]] virtual Count &GetCount(const std::string_view &) noexcept = 0;

  virtual void Transact(std::function<void()>) noexcept = 0;

  [[nodiscard]] virtual std::vector<std::string_view> Keys() const noexcept = 0;

  static std::unique_ptr<Bucket> Make();

  template <class T>
    requires bucket::Supported<T, String, Count>
  inline T &Get(const std::string_view &) noexcept;

  BUCKET_GET(String, GetString)
  BUCKET_GET(Count, GetCount)
};

} // namespace kvd

#undef BUCKET_GET
