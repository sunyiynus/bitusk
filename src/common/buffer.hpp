#ifndef BITUSK_BUFFER_HPP
#define BITUSK_BUFFER_HPP

namespace bitusk {

template <typename T>
class allocator;



template <template T>
class basic_buffer
{
private:
    T *buf_ptr;
    
public:
    basic_buffer() {}

};



} // namespace bitusk


#endif // BITUSK_BUFFER_HPP