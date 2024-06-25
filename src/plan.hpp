#pragma once

#include <sstream>

#include "core.hpp"

namespace kvd {

class Plan {
  KVD_PROTO(Plan);

public:
  virtual void Explain(std::ostream &) const = 0;

  [[nodiscard]] std::string Explain() const {
    std::ostringstream os;
    Explain(os);
    return os.str();
  }
};

} // namespace kvd
