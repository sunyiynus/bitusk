#ifndef BITUSK_ALLOCATOR_HPP
#define BITUSK_ALLOCATOR_HPP

namespace bitusk {

template <typename T>
class allocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
};

template <>
class allocator<void>
{

};



} // namespace bitusk


#endif // BITUSK_ALLOCATOR_HPP