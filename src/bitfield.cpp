#include "bitfield.hpp"
#include <cstddef>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <string>



NotFindException::NotFindException(const char* message):msg_(message) {}


NotFindException::NotFindException(const std::string& message): msg_(message) {}


const char* NotFindException::what() const noexcept{
    return msg_.c_str();
}


BitMap::BitMap(size_t bits, const bool initVal)
{

}


BitMap::BitMap(const std::string& bits) {
    std::istringstream ins (bits);
}


void BitMap::Clear() {
}

const size_t BitMap::Size() const {
}


bool BitMap::GetBitValue(int index) const {
}


BitMap& BitMap::SetBitValue(int index, bool val) {
    return *this;
}


BitMap& BitMap::SetAllBits(bool val) {
    return *this;
}


size_t BitMap::Counting(bool bit) const {
}


bool BitMap::operator==(const BitMap& a) {
}


size_t BitMap::DifferentPosition(const BitMap& cbitmap){
    if( *this == cbitmap ) throw NotFindException("Not find different bit");
    int i = 0;
#ifdef VECTOR_BOOL_SUPPORT_ITERTOR
    auto is_diff = [&](auto& ele) {
        return cbitmap.GetBitValue(i++) == ele;
    }
    auto result = std::find_if(bitmap_.begin(), bitmap_.end(), is_diff);
    // TODO
    // there may be error in this case :
    // result == bitmap_.begin() so std::distance(begin(), begin()) what happened?
    return std::distance(bitmap_.begin(), result) - 1;
#else
    if( bitmap_.size() != cbitmap.Size()) throw NotFindException("bit map size is different.");
    for( int i = 0; i < bitmap_.size(); ++i) {
        if( bitmap_[i] != cbitmap.GetBitValue(i)) {
            return i;
        }
    }

    throw NotFindException("Is The Same Bitmap.");
#endif
}

#define FOR_EACH(data) for(int i = 0; i < data; ++i)


void BitMap::Write(std::ostream& os) const {
    FOR_EACH(bitmap_.size()) {
        os << bitmap_[i] << " ";
    }
}

void BitMap::Read(std::istream& is) {
    bitmap_.clear();
    bool tmp = false;
    while( is >> tmp ) {
        bitmap_.push_back(tmp);
    }
}

std::ostream& operator<<(std::ostream& out, const BitMap& bitmap) {
    bitmap.Write(out);
    return out;
}

std::istream& operator>>(std::istream& in, BitMap& bitmap) {
    bitmap.Read(in);
    return in;
}
