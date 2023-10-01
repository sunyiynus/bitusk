#ifndef BITUSK_SRC_BITFIELD_H__
#define BITUSK_SRC_BITFIELD_H__

#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "common.hpp"

class NotFindException : public std::exception {
 public:
  explicit NotFindException(const char* message);
  explicit NotFindException(const std::string& message);
  virtual ~NotFindException() = default;

  virtual const char* what() const noexcept;

 protected:
  std::string msg_;
};

constexpr std::size_t BYTE_BITS = 8;
constexpr int32_t ZERO = 0;
constexpr int32_t ONE = 1;

class BitMap : public NotEqual<BitMap> {
 public:
  using bytes_type = std::uint8_t;
  BitMap() = default;
  BitMap(const BitMap& bm) = default;
  BitMap& operator=(const BitMap& bm) = default;
  ~BitMap();

  BitMap(size_t bits, const bool initVal);
  BitMap(const std::string& bits);

  const size_t Size() const;
  bool GetBitValue(int index) const;
  BitMap& SetBitValue(int index, bool val);
  BitMap& SetAllBits(bool val);
  void flip(void);
  bool operator==(const BitMap& a);

 public:
  static std::pair<std::size_t, std::size_t> bitPos(const size_t index);

 private:
  std::size_t bitSize;
  std::size_t uint8Size;
  bytes_type* bitsPtr;
};

std::ostream& operator<<(std::ostream& out, const BitMap& bitmap);
std::istream& operator>>(std::istream& in, BitMap& bitmap);

#endif
