#ifndef TYPE_TRAITS_HPP
#define TYPE_TRAITS_HPP

namespace mp{


template <typename T, T val>
struct integral_constant {
    static constexpr T value = val;
    using value_type = T;
    using type = integral_constant;
    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template <typename T1, typename T2>
struct is_same;
template <typename T1>
struct is_same<T1, T1>: true_type { };
template <typename T, typename U>
struct is_same<T, U>: false_type { };



} // namespace mp

#endif