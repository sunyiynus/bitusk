#include "bitfield.hpp"
#include <pthread.h>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

NotFindException::NotFindException(const char* message) : msg_(message) {}

NotFindException::NotFindException(const std::string& message)
    : msg_(message) {}

const char* NotFindException::what() const noexcept {
  return msg_.c_str();
}

BitMap::BitMap(size_t bits, const bool initVal) : bitSize(bits) {
  uint8Size =
      bits % BYTE_BITS == ZERO ? bits / BYTE_BITS : bits / BYTE_BITS + ONE;

  if (bitsPtr == nullptr) {
    bitsPtr = new bytes_type[uint8Size];
  }
  bytes_type tmp = 0;
  if (initVal) {
    tmp = static_cast<bytes_type>(ZERO) - ONE;
  }
  std::memset(bitsPtr, uint8Size, tmp);
}

BitMap::BitMap(const std::string& bitstr) : BitMap(bitstr.size(), false) {
  for (int i = 0; i < bitstr.size(); ++i) {
    bool val = true;
    if (bitstr[i] == '0') {
      val = false;
    }
    SetBitValue(i, val);
  }
}

BitMap::~BitMap() {
  delete[] bitsPtr;
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

std::ostream& operator<<(std::ostream& out, const BitMap& bitmap) {
  return out;
}

std::istream& operator>>(std::istream& in, BitMap& bitmap) {
  return in;
}
