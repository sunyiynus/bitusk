#ifndef __BITUSK_SRC_UTILITIES_HPP
#define __BITUSK_SRC_UTILITIES_HPP

#include <string>
#include <iostream>
#include <iomanip>



#define MEM_FN(x)       boost::bind(&SelfType::x, shared_from_this())
#define MEM_FN1(x,y)    boost::bind(&SelfType::x, shared_from_this(), y)
#define MEM_FN2(x,y,z)  boost::bind(&SelfType::x, this, y,z)
//#define MEM_FN2(x,y,z)  boost::bind(&SelfType::x, this, y,z)
#define MEM_FN3(x,y,z,m)  boost::bind(&SelfType::x, shared_from_this(), y, z, m)
#define MEM_FN4(x,y,z,m,p)  boost::bind(&SelfType::x, shared_from_this(), y, z, m, p)

#define LOGGER(x) LoggerStream x(Logger::Instance())
#define DEBUG_MACRO

namespace bitusk{

inline const std::string ConvertUstringToString(const std::basic_string<unsigned char>& src) {
    return reinterpret_cast<const char*>(src.c_str());
}

inline const std::basic_string<unsigned char> ConvertStringToUstring(const std::string& src) {
    return reinterpret_cast<const unsigned char*>(src.c_str());
}


static const std::string PrintUstring(const std::basic_string<unsigned char>& src){
    std::basic_ostringstream<char> buf;
    const unsigned int* digest = reinterpret_cast<const unsigned int*>(src.c_str());
    for( int i = 0; i < 5; ++i) {
        buf << std::hex << std::setfill('0') << std::setw(8) << digest[i];
    }
    return buf.str();
}

} //namespace bitusk


#endif // __BITUSK_SRC_UTILITIES_HPP