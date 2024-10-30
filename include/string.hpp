#ifndef BITUSK_STRING_HPP
#define BITUSK_STRING_HPP
#include <cstddef>
#include <cstring>

namespace bitusk {

constexpr std::size_t STRING_LOCAL_BUFFER_SIZE = 22;


template <typename CharT>
class BasicString {
public:

    typedef std::size_t size_type;

private:
    size_type len;
    CharT *bufPtr;
    CharT localBuffer[STRING_LOCAL_BUFFER_SIZE + 1];


public:
    BasicString(const CharT* const str);
    BasicString(const BasicString<CharT>& str);
    BasicString(BasicString<CharT>&& str);
public:

    

};

template<typename CharT>
inline BasicString<CharT>::BasicString(const CharT* const str) {
    size_type size = strlen(str);
    if (size > STRING_LOCAL_BUFFER_SIZE) {

    }
}

using String = BasicString<char>;




} // namespace bitusk


#endif // BITUSK_STRING_HPP