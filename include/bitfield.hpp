#ifndef BITUSK_SRC_BITFIELD_H__
#define BITUSK_SRC_BITFIELD_H__

#include <cstddef>
#include <exception>
#include <iostream>
#include <vector>
#include <string>

class NotFindException: public std::exception {
public:
    explicit NotFindException(const char* message);
    explicit NotFindException(const std::string& message);
    virtual ~NotFindException() = default;

    virtual const char* what() const noexcept;
protected:
    std::string msg_;
};

class BitMap {
public:
    BitMap() = default;
    BitMap(const BitMap& bm) = default;
    BitMap& operator=(const BitMap& bm) = default;
    ~BitMap() = default;

    BitMap(size_t bits);
    BitMap(const std::string& bits);

    void Clear();
    size_t Size() const;
    bool GetBitValue(int index) const ;
    BitMap& SetBitValue(int index, bool val);
    BitMap& SetAllBits(bool val);
    //BitMap& RestoreBitmap(const std::string& filepath);
    size_t Counting(bool bit) const;
    bool operator==(const BitMap& a);
    size_t DifferentPosition(const BitMap& cbitmap);

    void Write(std::ostream& os) const ;
    void Read(std::istream& is);

private:
    std::vector<bool> bitmap_;
};

std::ostream& operator<<(std::ostream& out, const BitMap& bitmap);
std::istream& operator>>(std::istream& in, BitMap& bitmap);


#endif