#pragma once

#include <string>

#include "core.hpp"

namespace kvd {

class Result {
  KVD_PROTO(Result);

public:
  [[nodiscard]] virtual const std::string &ToString() const noexcept = 0;
};

} // namespace kvd
