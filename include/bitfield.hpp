#ifndef BITUSK_SRC_BITFIELD_H__
#define BITUSK_SRC_BITFIELD_H__

#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <vector>
#include <string>
#include "common.hpp"

class NotFindException: public std::exception {
public:
    explicit NotFindException(const char* message);
    explicit NotFindException(const std::string& message);
    virtual ~NotFindException() = default;

    virtual const char* what() const noexcept;
protected:
    std::string msg_;
};


class BitMap: public NotEqual<BitMap> {
public:
    BitMap() = default;
    BitMap(const BitMap& bm) = default;
    BitMap& operator=(const BitMap& bm) = default;
    ~BitMap() = default;

    BitMap(size_t bits, const bool initVal);
    BitMap(const std::string& bits);

    void Clear();
    const size_t Size() const;
    bool GetBitValue(int index) const ;
    BitMap& SetBitValue(int index, bool val);
    BitMap& SetAllBits(bool val);
    size_t Counting(bool bit) const;
    bool operator==(const BitMap& a);

private:
    std::size_t uint8Size;
    std::uint8_t* bitsPtr;
};



std::ostream& operator<<(std::ostream& out, const BitMap& bitmap);
std::istream& operator>>(std::istream& in, BitMap& bitmap);


#endif
