
#include <string>
#include <sstream>
#include <iomanip>


namespace bitusk{

static const std::string ConvertUstringToString(const std::basic_string<unsigned char>& src){
    std::basic_ostringstream<char> buf;
    const unsigned int* digest = reinterpret_cast<const unsigned int*>(src.c_str());
    for( int i = 0; i < 5; ++i) {
        buf << std::hex << std::setfill('0') << std::setw(8) << digest[i];
    }
    return buf.str();
}

}

