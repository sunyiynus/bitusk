#ifndef COM_FORMAT_HPP
#define COM_FORMAT_HPP

#include "types.h"
namespace fmt {

    void printf("name {}", var);

template <typename T>
auto to_string(T var) -> std::string;

template<>
auto to_string<char>(char var) -> std::string
{
    return std::string(var);
}

template<>
auto to_string<int>(int var) -> std::string
{
}


template <typename Char, typename ...Types>
struct formatter;

template <typename Char, typename T, typename ...Types>
struct formatter<Char, T, Types...>:: public formatter<Char, Types...> {
    auto format(Char* cstr, T var, Types... args) 
};

template <typename Char, typename T>
struct formatter {
    auto format(Char* cstr, T arg) -> void {
    }

};

template <typename ...Ts>
std::string format_var(Ts... args);

template <typename T>
std::string format_var(T var) {
    std::string res;
    if constexpr (std::is_same<char, T>::value || std::is_same<const char*, T>::value){
        res += (var);
    } else {
        res += std::to_string(var);
    }
    return res;
}

template <typename T, typename ...Ts>
std::string format_var(T var, Ts... args) {
    std::string res;
    if constexpr (std::is_same<char, T>::value || std::is_same<const char*, T>::value){
        res += (var);
    } else {
        res += std::to_string(var);
    }
    res += format_var(args...);
    return res;
}

} //




#endif // COM_FORMAT_HPP