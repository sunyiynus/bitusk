
#ifndef BITUSK_CSTRING_HPP
#define BITUSK_CSTRING_HPP
#include <cstddef>
#include <iterator>

namespace bitusk {

constexpr char EOF = '\0';

inline std::size_t strlen(const char* str) 
{
    const char* char_ptr;
    unsigned long int longword, himagic, lomagic;
    const unsigned long int * longword_ptr;
    for (char_ptr = str; ((const unsigned long int*)char_ptr & 
                (sizeof(longword) - 1)) != 0; char_ptr++) {
        if (*char_ptr == EOF) {
            return char_ptr - str;
        }
    }

    longword_ptr = (const unsigned long int*) char_ptr;
    lomagic = 0x01010101;
    himagic = 0x80808080;
    if (sizeof(longword) > 4) {
        lomagic = ((lomagic << 16) << 16) | lomagic;
        himagic = ((himagic << 16) << 16) | himagic;

    }

    for(;;) {
        longword = *longword_ptr++;
        if (((longword - lomagic) & ~longword & himagic) != 0) {
            const char *cp = (const char* ) (longword_ptr - 1);
            if (cp[0] == 0) return cp - str;
            if (cp[1] == 0) return cp - str + 1;
            if (cp[2] == 0) return cp - str + 2;
            if (cp[3] == 0) return cp - str + 3;
            if (sizeof(longword) > 4) {
                if (cp[4] == 0) return cp - str + 4;
                if (cp[5] == 0) return cp - str + 5;
                if (cp[6] == 0) return cp - str + 6;
                if (cp[7] == 0) return cp - str + 7;
            }
        }
    }
    return 0;
}


inline int strcpy(char* dst, const std::size_t dst_size, const char* src, const std::size_t src_size)
{
    if (dst == nullptr || src == nullptr) {
        return -1;
    }

    for (std::size_t idx = 0; idx < dst_size && idx < src_size; idx++) {
        dst[idx] = src[idx];
    }

    return 0;
}



} // namespace bitusk


#endif // BITUSK_CSTRING_HPP
