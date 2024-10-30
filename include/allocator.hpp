#ifndef BITUSK_ALLOCATOR_HPP
#define BITUSK_ALLOCATOR_HPP

#include <cstddef>
#include <exception>
#include "mem.h"



namespace bitusk {


template <typename T>
class allocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    // allocator() noexcept = default;
    constexpr allocator() noexcept = default;
    allocator(const allocator& other) noexcept = default;

    constexpr T* allocate(std::size_t n) {
        T* obj = static_cast<T*>(smalloc(sizeof(T) * n));
        return obj;
    }

    template<typename ...Args>
    constexpr T* allocate(std::size_t n, Args&& ... args) {
        T* obj = static_cast<T*>(smalloc(sizeof(T) * n));
        for (std::size_t i = 0; i < n; ++i) {
            new (&obj[i]) T(std::forward<Args>(args)...);
        }
        return obj;
    }
    void deallocate(T* p, std::size_t n) {
        for (std::size_t i = 0; i < n; ++i) {
            p[i].~T();
        }
        sfree(p);
    }

};


template <>
class allocator<void> {};



} // namespace bitusk


#endif // BITUSK_ALLOCATOR_HPP