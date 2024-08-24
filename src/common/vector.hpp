#ifndef BITUSK_VECTOR_HPP
#define BITUSK_VECTOR_HPP
#include <cstddef>
#include <cstring>

namespace bitusk {

constexpr std::size_t INIT_CAPACITY = 2;

template <typename T>
class vector {
public:

    typedef std::size_t size_type;

private:
    size_type len;
    size_type capacity;
    T* bufferPtr;




public:
    vector();
    vector(const vector<T>& str);
    vector(vector<T>&& str);
    ~vector();
    void push_back(const T& val);

private:
    void expend_capacity();

};

template<typename T>
vector<T>::vector()
{
    len = 0;
    capacity = INIT_CAPACITY;
    bufferPtr = static_cast<T*>(std::malloc(sizeof(T) * capacity));
}

template<typename T>
vector<T>::~vector()
{
    if (bufferPtr) {
        std::free(bufferPtr);
        bufferPtr = nullptr;
        len = 0;
        capacity = 0;
    }
}


template<typename T>
void vector<T>::expend_capacity()
{
    
}

} // namespace bitusk





#endif // BITUSK_VECTOR_HPP