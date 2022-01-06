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
    constexpr int kUnInterested = 3;
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
    static int IntToChar(int i, uostringstream& os);
    static int IntToChar(int i, unsigned char* c);
    static int CharToInt(const unsigned char* c);
    static const std::string ConvertUstringToString(const std::basic_string<unsigned char>& src);

    static const std::string CreateHandShakedMsg(Peer& myself, Peer& peer);
    static const std::string CreateKeepAliveMsg(Peer& myself, Peer& peer);
    static const std::string CreateChokedMsg(Peer& myself, Peer& peer);
    static const std::string CreateUnchokedMsg(Peer& myself, Peer& peer);
    static const std::string CreateInterestedMsg(Peer& myself, Peer& peer);
    static const std::string CreateUninterestedMsg(Peer& myself, Peer& peer);
    static const std::string CreateHaveMsg(Peer& myself, Peer& peer);

};


class AbsMessageGenerator: public MsgTyper{
public:
    virtual const ustring GenerateMessage(Peer& peer) {}
    virtual AbsMessageGenerator& Set(std::vector<int>& item){}
protected:
    uostringstream msg;
};

class AbsMessageParser : public MsgTyper{
public:
    static bool IsCompleteMsg(const ustring& buff, int ok_size){}
    virtual void ParseMessage(Peer& peer, const ustring& msg){}
};

class AbsMsgFactory{
public:
    virtual AbsMessageGenerator* GetMsgGenerator(Peer& peer, int msg_type){}
    virtual AbsMessageParser* GetMsgParser(Peer& peer){}
};


class MsgGeneratorFactory: public AbsMsgFactory{
public:
    virtual AbsMessageGenerator* GetMsgGenerator(Peer& peer, int msg_type) override;                                                                                                                                                                  
    
};


class MsgParserFactory: public AbsMsgFactory{
public:
    virtual AbsMessageParser* GetMsgParser(Peer& peer) override;
};

// 小类太多，没有任何意义？？？
// 假设的情况：以后需要添加新的msg类型。

// message handle of trakcers

class TrackerRequestGenerator: public AbsMessageGenerator{
public:
    virtual const ustring GenerateMessage(Peer& peer) override;
    virtual TrackerRequestGenerator& Set(std::vector<int>& item) override;
    void Set(const std::string& tname);
protected:
    uostringstream msg;
    std::vector<int> items_;
    std::string tracker_name_;
};


class TrackerMsgFactory: public AbsMsgFactory {
public:
    virtual AbsMessageGenerator* GetMsgGenerator(Peer& peer, int msg_type){}
    virtual AbsMessageParser* GetMsgParser(Peer& peer){}
};

// First Party of this mudel
// Peer data exchange needed message generator

class HandShakeMsg: public AbsMessageGenerator {
public:
    virtual const ustring GenerateMessage(Peer& peer) override;
};


class KeepAliveMsg: public AbsMessageGenerator {
public:
    virtual const ustring GenerateMessage(Peer& peer) override;
};


class ChokedMsg: public AbsMessageGenerator {
public:
    virtual const ustring GenerateMessage(Peer& peer) override;
};


class UnChockdMsg: public AbsMessageGenerator {
public:
    virtual const ustring GenerateMessage(Peer& peer) override;
};

class InterestedMsg: public AbsMessageGenerator {
public:
    virtual const ustring GenerateMessage(Peer& peer) override;
};


class UnInterestedMsg: public AbsMessageGenerator {
public:
    virtual const ustring GenerateMessage(Peer& peer) override;
};



class HaveMsg: public AbsMessageGenerator {
public:
    virtual const ustring GenerateMessage(Peer& peer) override;
    virtual HaveMsg& Set(std::vector<int>& item) override;
private:
    int index_;
};


class BitfieldMsg: public AbsMessageGenerator {
public:
    virtual const ustring GenerateMessage(Peer& peer) override;
    void ConvertBitmap(Peer& peer, uostringstream& os);
};


class RequestMsg: public AbsMessageGenerator {
public:
    virtual const ustring GenerateMessage(Peer& peer) override;
    virtual RequestMsg& Set(std::vector<int>& item) override;
    
};


class PieceMsg: public AbsMessageGenerator {
public:
    virtual const ustring GenerateMessage(Peer& peer) override;
};


class CancelMsg: public AbsMessageGenerator {
public:
    virtual const ustring GenerateMessage(Peer& peer) override;
    virtual CancelMsg& Set(std::vector<int>& item) override;
};


class PortMsg: public AbsMessageGenerator {
public:
    virtual const ustring GenerateMessage(Peer& peer) override;
};


// Second Part of Message Module
class ProcessHandShakeMsg: public AbsMessageParser {
public:
    virtual void ParseMessage(Peer& peer, const ustring& msg) override;
};


class ProcessKeepAliveMsg: public AbsMessageParser {
public:
    virtual void ParseMessage(Peer& peer, const ustring& msg) override;
};


class ProcessChokeMsg: public AbsMessageParser {
public:
    virtual void ParseMessage(Peer& peer, const ustring& msg) override;
};

/*
class ProcessUnchokeMsg: public AbsMessageParser {
public:
    virtual void ParseMessage(Peer& peer, const ustring& msg) override;
};


class ProcessInterestedMsg: public AbsMessageParser {
public:
    virtual void ParseMessage(Peer& peer, const ustring& msg) override;
};


class ProcessUninterestedMsg: public AbsMessageParser {
public:
    virtual void ParseMessage(Peer& peer, const ustring& msg) override;
};


class ProcessHaveMsg: public AbsMessageParser {
public:
    virtual void ParseMessage(Peer& peer, const ustring& msg) override;
};


class ProcessCancelMsg: public AbsMessageParser {
public:
    virtual void ParseMessage(Peer& peer, const ustring& msg) override;
};


class ProcessBitfieldMsg: public AbsMessageParser {
public:
    virtual void ParseMessage(Peer& peer, const ustring& msg) override;
};


class ProcessRequestMsg: public AbsMessageParser {
public:
    virtual void ParseMessage(Peer& peer, const ustring& msg) override;
};


class ProcessPieceMsg: public AbsMessageParser {
public:
    virtual void ParseMessage(Peer& peer, const ustring& msg) override;
};
*/


#endif // BITUSK_SRC_MESSAGE_HPP__