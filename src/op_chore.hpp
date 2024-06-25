#pragma once

#include <array>
#include <cstddef>
#include <cstring>
#include <memory>
#include <ostream>
#include <string_view>
#include <utility>
#include <vector>

#include "op.hpp"

namespace kvd {

template <std::size_t N, class Cp> struct OpName {
  std::array<Cp, N> name;
  constexpr OpName(const Cp (&pp)[N]) : name{OpArr(pp)} {} // NOLINT

  template <class Tp, std::size_t Sz, std::size_t... I>
  inline constexpr std::array<std::remove_cv_t<Tp>, Sz>
  OpArr(Tp (&a)[Sz], std::index_sequence<I...>) { // NOLINT
    return {{a[I]...}};
  }

  template <class Tp, std::size_t Sz>
  inline constexpr std::array<std::remove_cv_t<Tp>, Sz> OpArr(Tp( // NOLINT
      &arr)[Sz]) noexcept(std::is_nothrow_constructible_v<Tp, Tp &>) {
    return OpArr(arr, std::make_index_sequence<Sz>());
  }
};

template <OpName op> constexpr auto operator""_OP() { return op.name; }

template <std::size_t Sz>
std::ostream &operator<<(std::ostream &os, const std::array<char, Sz> arr) {
  os << arr.data();
  return os;
}

template <auto Name, std::size_t Nargs> class OpTraits : public Op {
public:
  static constexpr std::array<char, Name.size()> op_name{std::move(Name)};
  static constexpr std::size_t op_name_size = op_name.size() - 1;
  static constexpr std::size_t op_nargs = Nargs;
};

template <auto OpName> class NullaryOp : public OpTraits<OpName, 0> {
public:
  explicit NullaryOp() = default;

  void Explain(std::ostream &os) const noexcept final { os << OpName; }
};

template <auto OpName> class UnaryOp : public OpTraits<OpName, 1> {
public:
  explicit UnaryOp(const std::string_view key) : key_{key} {}

  void Explain(std::ostream &os) const noexcept final {
    os << OpName << ':' << key_;
  }

protected:
  std::string_view key_; // NOLINT
};

template <auto OpName> class BinaryOp : public OpTraits<OpName, 2> {
public:
  explicit BinaryOp(const std::string_view key, const std::string_view value)
      : key_{key}, value_{value} {}

  void Explain(std::ostream &os) const noexcept final {
    os << OpName << ':' << key_ << ',' << value_;
  }

protected:
  std::string_view key_, value_; // NOLINT
};

/* OpsForeach */
template <class O, std::size_t... N>
std::unique_ptr<Op> OpMake(const std::vector<std::string_view> &args,
                           std::index_sequence<N...> /**/) {
  return std::make_unique<O>(args[N]...);
}

template <class O, std::size_t N>
std::unique_ptr<Op> OpMake(const std::vector<std::string_view> &args) {
  return OpMake<O>(args, std::make_index_sequence<N>{});
}

template <class...> struct OpList {};
template <class> struct OpsIt;

template <class O, class... Os> struct OpsIt<OpList<O, Os...>> {
  static std::unique_ptr<Op> Apply(const std::string_view &name,
                                   const std::vector<std::string_view> &args) {
    if (not std::strncmp(name.data(), O::op_name.data(), O::op_name_size)) {
      return OpMake<O, O::op_nargs>(args);
    }
    return OpsIt<OpList<Os...>>::Apply(name, args);
  }
};

template <> struct OpsIt<OpList<>> {
  static std::unique_ptr<Op> Apply(const std::string_view & /**/,
                                   const std::vector<std::string_view> & /**/) {
    static_assert(sizeof(bool) != 0,
                  "op_name does not define a valid listed Op.");
    return nullptr;
  }
};

} // namespace kvd
