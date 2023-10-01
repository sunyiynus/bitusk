#include "bitfield.hpp"
#include <pthread.h>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

NotFindException::NotFindException(const char* message) : msg_(message) {}

NotFindException::NotFindException(const std::string& message)
    : msg_(message) {}

const char* NotFindException::what() const noexcept {
  return msg_.c_str();
}

BitMap::BitMap() : bitSize(ZERO), uint8Size(ZERO), bitsPtr(nullptr) {}
BitMap::BitMap(const BitMap& bm) {
  bitSize = bm.bitSize;
  uint8Size = bm.uint8Size;
}

BitMap::BitMap(BitMap&& bm): bitsPtr(nullptr) {
  this->bitsPtr = bm.bitsPtr;
  bitSize = bm.bitSize;
  uint8Size = bm.uint8Size;
  bm.bitsPtr = nullptr;
  bm.bitSize = ZERO;
  bm.uint8Size = ZERO;
}

BitMap::BitMap(size_t bits, const bool initVal)
    : bitSize(bits), bitsPtr(nullptr) {
  uint8Size =
      bits % BYTE_BITS == ZERO ? bits / BYTE_BITS : bits / BYTE_BITS + ONE;

  bitsPtr = new bytes_type[uint8Size];
  bytes_type tmp = 0;
  if (initVal) {
    tmp = static_cast<bytes_type>(ZERO) - ONE;
  }
  std::memset(bitsPtr, uint8Size, tmp);
}

BitMap::~BitMap() {
  if (bitsPtr != nullptr) {
    delete[] bitsPtr;
  }
}

const size_t BitMap::Size() const {
  return bitSize;
}

bool BitMap::GetBitValue(int index) const {
  if (index >= bitSize) {
    throw std::out_of_range("Bitmap Index out range.");
  }
  auto pos = bitPos(index);
  return ((bitsPtr[pos.first] & (ONE << pos.second)) > ZERO);
}

BitMap& BitMap::SetBitValue(int index, bool val) {
  if (index >= bitSize) {
    return *this;
  }

  auto pos = bitPos(index);
  if (val == true) {
    bitsPtr[pos.first] |= (ONE << pos.second);
  } else {
    bitsPtr[pos.first] &= ~(ONE << pos.second);
  }
  return *this;
}

BitMap& BitMap::SetAllBits(bool val) {
  bytes_type tmp = ZERO;
  if (val) {
    tmp = static_cast<bytes_type>(ZERO) - ONE;
  }
  std::memset(bitsPtr, uint8Size, tmp);
  return *this;
}

void BitMap::flip() {
  for (int i = 0; i < uint8Size; ++i) {
    bitsPtr[i] = ~bitsPtr[i];
  }
}

bool BitMap::operator==(const BitMap& a) {
  if (a.Size() != this->Size()) {
    return false;
  } else {
    for (int i = 0; i < uint8Size; i++) {
      if (a.bitsPtr[i] != this->bitsPtr[i]) {
        return false;
      }
    }
    return true;
  }
}

std::pair<std::size_t, std::size_t> BitMap::bitPos(const size_t index) {
  std::pair<std::size_t, std::size_t> res;
  res.first = index / BYTE_BITS;
  res.second = index % BYTE_BITS;
  return res;
}

BitMap ConvertBitsStringToBitMap(const std::string& s) {
  std::string tmp;
  for (auto c : s) {
    if (c == '0' || c == '1') {
      tmp += c;
    }
  }
  BitMap bitmap(tmp.size(), false);
  for (int i = 0; i < tmp.size(); ++i) {
    bool val = true;
    if (tmp[i] == '0') {
      val = false;
    }
    bitmap.SetBitValue(i, val);
  }
  return bitmap;
}

std::ostream& operator<<(std::ostream& out, const BitMap& bitmap) {
  return out;
}

std::istream& operator>>(std::istream& in, BitMap& bitmap) {
  return in;
}
