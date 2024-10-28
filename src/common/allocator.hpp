#ifndef BITUSK_ALLOCATOR_HPP
#define BITUSK_ALLOCATOR_HPP

#include "mem.h"

namespace bitusk {

void* operator new(std::size_t n)
{
    void* p = smalloc(n);
    if (!p) throw std::bad_alloc();
    return p;
}

template <typename T>
class allocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    allocator() throw ();
    allocator() noexcept;
    constexpr allocator() noexcept;
    allocator(const allocator& other) throw();

    constexpr T* allocate(std::size_t n);
    void deallocate(T* p, std::size_t n);
private:
    T* _ptr;

};

template<typename T>
constexpr T* allocator<T>::allocate(std::size_t n)
{
    T* obj = static_cast<T*>(operator new(sizeof(T) * n));
    new (obj) T();
}

template <>
class allocator<void>
{

};



} // namespace bitusk


#endif // BITUSK_ALLOCATOR_HPP