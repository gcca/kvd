#pragma once

#include <type_traits>

namespace kvd::bucket {

template <class T, class... Ts> struct TSupported;
template <class T> struct TSupported<T> : std::false_type {};

template <class T, class... Ts>
static inline constexpr bool Supported = TSupported<T, Ts...>::value;

template <class T, class U, class... Ts>
struct TSupported<T, U, Ts...>
    : std::bool_constant<std::is_same_v<T, U> or Supported<T, Ts...>> {};

} // namespace kvd::bucket
