#ifndef TYPE_TRAITS_HPP
#define TYPE_TRAITS_HPP


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


#endif