#ifndef BITUSK_SRC_MESSAGE_HPP__
#define BITUSK_SRC_MESSAGE_HPP__

#include "peer.hpp"
#include <sstream>
#include <string>
#include <mutex>

class Peer;



namespace MsgNum {
    constexpr int kHandShake = -2;
    constexpr int kKeepAlive = -1;
    constexpr int kChoke = 0;
    constexpr int kUnchoke = 1;
    constexpr int kInterested = 2;
    constexpr int kUninterested = 3;
    constexpr int kHave = 4;
    constexpr int kBitfield = 5;
    constexpr int kRequest = 6;
    constexpr int kPiece = 7;
    constexpr int kCancel = 8;
    constexpr int kPort = 9;
};

class MsgTyper {
public:
    typedef std::basic_string<unsigned char> ustring;
    typedef std::basic_stringstream<unsigned char> ustringstream;
    typedef std::basic_istringstream<unsigned char> uistringstream;
    typedef std::basic_ostringstream<unsigned char> uostringstream;

    static ustring keep_alive    ;
    static ustring chocke        ;
    static ustring unchocke      ;
    static ustring interested    ;
    static ustring uninterested  ;
    static ustring have          ;
    static ustring request       ;
    static ustring cancel        ;
    static ustring port          ;
    static std::map<int, boost::function<bool(Peer&,Peer&, const ustring&)>> processors;


    static int IntToChar(int i, uostringstream& os);
    static int IntToChar(int i, unsigned char* c);
    static int CharToInt(const unsigned char* c);
    static long long CharToInt(const ustring& str);
    static const std::string ConvertUstringToString(const std::basic_string<unsigned char>& src);

    static void InitMsgProcessors();

    static const std::string CreateMsg(Peer& myself, Peer& peer);
    static const std::string CreateHandShakedMsg(Peer& myself, Peer& peer);
    static const std::string CreateKeepAliveMsg(Peer& myself, Peer& peer);
    static const std::string CreateChokedMsg(Peer& myself, Peer& peer);
    static const std::string CreateUnchokedMsg(Peer& myself, Peer& peer);
    static const std::string CreateInterestedMsg(Peer& myself, Peer& peer);
    static const std::string CreateUninterestedMsg(Peer& myself, Peer& peer);
    static const std::string CreateHaveMsg(Peer& myself, Peer& peer, size_t index);
    static const std::string CreateBitfieldMsg(Peer& myself, Peer& peer);
    static const std::string CreatePieceMsg(Peer& myself, Peer& peer);
    static const std::string CreateCancelMsg(Peer& myself, Peer& peer);
    static const std::string CreatePortMsg(Peer& myself, Peer& peer);

    static std::vector<ustring> SplitMsg(Peer& myself, Peer& peer);

    static bool ParseMsg_map(Peer& myself, Peer& peer);
    static bool ParseMsg(Peer& myself, Peer& peer);
    static bool ProcessHandShakeMsg(Peer& myself, Peer& peer, const ustring& str);
    static bool ProcessKeepAliveMsg(Peer& myself, Peer& peer, const ustring& str);
    static bool ProcessChokeMsg(Peer& myself, Peer& peer, const ustring& str);
    static bool ProcessUnchokeMsg(Peer& myself, Peer& peer, const ustring& str);
    static bool ProcessInterestedMsg(Peer& myself, Peer& peer, const ustring& str);
    static bool ProcessUninterestedMsg(Peer& myself, Peer& peer, const ustring& str);
    static bool ProcessHaveMsg(Peer& myself, Peer& peer, const ustring& str);
    static bool ProcessBitfieldMsg(Peer& myself, Peer& peer, const ustring& str);
    static bool ProcessRequestMsg(Peer& myself, Peer& peer, const ustring& str);
    static bool ProcessPieceMsg(Peer& myself, Peer& peer, const ustring& str);
    static bool ProcessCancelMsg(Peer& myself, Peer& peer, const ustring& str);
    static bool ProcessPortMsg(Peer& myself, Peer& peer, const ustring& str);

};


inline int MsgTyper::IntToChar(int i, MsgTyper::uostringstream& os) {
    unsigned char c [4] = {0};
    IntToChar(i, c);
    os << c[0] << c[1] << c[2] << c[3];
    return 0;
}


inline int MsgTyper::IntToChar(int i, unsigned char* c) {
    c[3] = (i % 256);
    c[2] = (i - c[3]) / 256 % 256;
    c[1] = (i - c[3] - c[2]*256) / 256 / 256 % 256;
    c[0] = (i - c[3] - c[2]*256 -c[1]*256*256) / 256 / 256 / 256 % 256;
    return 0;
}


inline int MsgTyper::CharToInt(const unsigned char * c) {
    int i = 0;
    i = c[0] * 256 * 256 * 256
      + c[1] * 256 * 256
      + c[2] * 256
      + c[3];

    return i;
}


inline long long MsgTyper::CharToInt(const ustring& str) {
    if( str.size() < 4 ) return 0;
    long long i = 0;
    i = str[0] * 256 * 256 * 256
      + str[1] * 256 * 256
      + str[2] * 256
      + str[3];

    return i;
}


#endif // BITUSK_SRC_MESSAGE_HPP__
