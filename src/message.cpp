#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iterator>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <queue>
#include <set>

#include <boost/function.hpp>

#include "message.hpp"
#include "basic.hpp"

#define UCHAR( x ) static_cast<unsigned char>( x )

MsgTyper::ustring MsgTyper::keep_alive    { 0x0, 0x0, 0x0, 0x0 };
MsgTyper::ustring MsgTyper::chocke        { 0x0, 0x0, 0x0, 0x1, 0x0};
MsgTyper::ustring MsgTyper::unchocke      { 0x0, 0x0, 0x0, 0x1, 0x1};
MsgTyper::ustring MsgTyper::interested    { 0x0, 0x0, 0x0, 0x1, 0x2};
MsgTyper::ustring MsgTyper::uninterested  { 0x0, 0x0, 0x0, 0x1, 0x3};
MsgTyper::ustring MsgTyper::have          { 0x0, 0x0, 0x0, 0x5, 0x4};
MsgTyper::ustring MsgTyper::request       { 0x0, 0x0, 0x0, 0xd, 0x6};
MsgTyper::ustring MsgTyper::cancel        { 0x0, 0x0, 0x0, 0xd, 0x8};
MsgTyper::ustring MsgTyper::port          { 0x0, 0x0, 0x0, 0x3, 0x9};
std::map<int, boost::function<bool(Peer&,Peer&, const MsgTyper::ustring&)>> MsgTyper::processors;





const std::string MsgTyper::CreateMsg(Peer& myself, Peer& peer) {

}

const std::string BasicCreateMsg(Peer& myself,
    Peer& peer, boost::function<void(Peer&, Peer&, std::basic_ostringstream<unsigned char>&)> func) {
    MsgTyper::uostringstream msg;
    func(myself, peer, msg);
    return bitusk::ConvertUstringToString(msg.str());
}

const std::string MsgTyper::CreateHandShakedMsg(Peer& myself, Peer& peer) {
    uostringstream msg;
    std::string name = "BitTorrent protocol";
    unsigned char pstrlen = 19;
    msg << pstrlen;
    msg << reinterpret_cast<const unsigned char*>(name.c_str());
    for(int i = 0; i < 8; ++i) msg.put(UCHAR(0));
           msg << myself.info_hash.c_str();
           msg << myself.peer_id.c_str();
    return bitusk::ConvertUstringToString(msg.str());
}



const std::string MsgTyper::CreateKeepAliveMsg(Peer& myself, Peer& peer) {
    uostringstream msg;
    msg << UCHAR(0x0);
    msg << UCHAR(0x0);
    msg << UCHAR(0x0);
    msg << UCHAR(0x0);
    return bitusk::ConvertUstringToString(msg.str());
}


const std::string MsgTyper::CreateChokedMsg(Peer& myself, Peer& peer) {
    uostringstream msg;
    IntToChar(1, msg );
    msg << UCHAR(0);
    return bitusk::ConvertUstringToString(msg.str());
}


const std::string MsgTyper::CreateUnchokedMsg(Peer& myself, Peer& peer) {
    uostringstream msg;
    IntToChar(1, msg );
    msg << UCHAR(1);
    return bitusk::ConvertUstringToString(msg.str());
}


const std::string MsgTyper::CreateInterestedMsg(Peer& myself, Peer& peer) {
    uostringstream msg;
    IntToChar(1, msg );
    msg << UCHAR(2);
    return bitusk::ConvertUstringToString(msg.str());
}


const std::string MsgTyper::CreateUninterestedMsg(Peer& myself, Peer& peer) {
    uostringstream msg;
    IntToChar(1, msg );
    msg << UCHAR(3);
    return bitusk::ConvertUstringToString(msg.str());
}


const std::string MsgTyper::CreateHaveMsg(Peer& myself, Peer& peer, size_t index) {
    uostringstream msg;
    IntToChar(5, msg );
    msg << UCHAR(4);
    IntToChar(index, msg );

    return bitusk::ConvertUstringToString(msg.str());
}


const std::string MsgTyper::CreateBitfieldMsg(Peer& myself, Peer& peer) {
    uostringstream msg;
    IntToChar(1 + ((myself.bitmap.Size()%8 == 0)? (myself.bitmap.Size() /8): (myself.bitmap.Size()/8 +1)),
                msg );
    msg << UCHAR(5);
    BitMap& bitmap = myself.bitmap;
    int charlen = (bitmap.Size() % 8 == 0) ? (bitmap.Size() / 8): (bitmap.Size() / 8 + 1);
    unsigned char* bits = new unsigned char[charlen];

    for(int i = 0; i < bitmap.Size(); ++i) {
        bits[i / 8] |=  1 << (i % 8);
    }
    ustring bitstring (bits, charlen);
    msg << bitstring;
    delete [] bits;
    return bitusk::ConvertUstringToString(msg.str());
}


const std::string MsgTyper::CreatePieceMsg(Peer& myself, Peer& peer) {
    // TODO
    uostringstream msg;
    IntToChar(1 + 12, msg);
    msg << UCHAR(7);

    return bitusk::ConvertUstringToString(msg.str());
}


const std::string MsgTyper::CreateCancelMsg(Peer& myself, Peer& peer) {
    // TODO
    uostringstream msg;
    IntToChar(1 + 12, msg);
    msg << UCHAR(6); // message type tag
    // TODO

    return bitusk::ConvertUstringToString(msg.str());
}


const std::string MsgTyper::CreatePortMsg(Peer& myself, Peer& peer) {
    uostringstream msg;
    IntToChar(1 + 12, msg);
    msg << UCHAR(6);
    // TODO

    return bitusk::ConvertUstringToString(msg.str());
}






void MsgTyper::InitMsgProcessors() {
    processors[MsgNum::kHandShake] = ProcessHandShakeMsg;
    processors[MsgNum::kKeepAlive] = ProcessKeepAliveMsg;
    processors[MsgNum::kChoke] = ProcessChokeMsg;
    processors[MsgNum::kUnchoke] = ProcessUnchokeMsg;
    processors[MsgNum::kInterested] = ProcessInterestedMsg;
    processors[MsgNum::kUninterested] = ProcessUninterestedMsg;
    processors[MsgNum::kHave] = ProcessHaveMsg;
    processors[MsgNum::kBitfield] = ProcessBitfieldMsg;
    processors[MsgNum::kRequest] = ProcessRequestMsg;
    processors[MsgNum::kPiece] = ProcessPieceMsg;
    processors[MsgNum::kCancel] = ProcessCancelMsg;
    processors[MsgNum::kPort] = ProcessPortMsg;
}


std::vector<MsgTyper::ustring> MsgTyper::SplitMsg(Peer& myself, Peer& peer) {

    ustring msg (reinterpret_cast<unsigned char*>(peer.read_buffer));
    std::memset(peer.read_buffer, 0, Peer::MaxBufferSize);
    ustring words;
    words += UCHAR(19);
    words += reinterpret_cast<const unsigned char*>("BitTorrent protocol");

    std::vector<ustring> msgs;

    int len = msg.size();
    for(int i = 0; i < len; ) {
        // 握手、chocke、have等消息的长度是固定的
        if( i+68<=len && msg.substr(i, 20) == words ) {
            msgs.push_back(msg.substr(i,68));
            i += 68;
        }
        else if( i+4 <=len && msg.substr(i, 4) == keep_alive ) {
            msgs.push_back(msg.substr(i, 5));
            i += 4;
        }
        else if( i+5 <=len && msg.substr(i, 5) == chocke  ) {
            msgs.push_back(msg.substr(i, 5));
            i += 5;
        }
        else if( i+5 <=len && msg.substr(i, 5) == unchocke ) {
            msgs.push_back(msg.substr(i, 5));
            i += 5;
        }
        else if( i+5 <=len && msg.substr(i, 5) == interested ) {
            msgs.push_back(msg.substr(i, 5));
            i += 5;
        }
        else if( i+5 <=len && msg.substr(i, 5) == uninterested ) {
            msgs.push_back(msg.substr(i, 5));
            i += 5;
        }
        else if( i+9 <=len && msg.substr(i, 5) == have ) {
            msgs.push_back(msg.substr(i, 5));
            i += 9;
        }
        else if( i+17<=len && msg.substr(i, 5) == request ) {
            msgs.push_back(msg.substr(i, 5));
            i += 17;
        }
        else if( i+17<=len && msg.substr(i, 5) == cancel ) {
            msgs.push_back(msg.substr(i, 17));
            i += 17;
        }
        else if( i+7 <=len && msg.substr(i, 5) == port ) {
            msgs.push_back(msg.substr(i, 7));
            i += 7;
        }
        // bitfield消息的长度是变化的
        else if( i+5 <=len && msg[i+4]== UCHAR(5) )  {
            size_t length = CharToInt(msg.substr(i, 4));
            if( i+4+length <= len ) {
                msgs.push_back(msg.substr(i, 4 + length));
                i += 4 + length;
            }
            else {
                return msgs;
            }
        }
        else if( i+5 <=len && msg[i+4]==7 )  {
            size_t length = CharToInt(msg.substr(i, 4));
            if( i+4+length <= len ) {
                msgs.push_back(msg.substr(i , 4 + length));
                i += 4+length;
            }
            else {
                return msgs;
            }
        }
        else {
            // 处理未知类型的消息
            if(i+4 <= len) {
                size_t length = CharToInt(msg.substr(i,4));
                if(i+4+length <= len)  {
                    i += 4+length;
                    msgs.push_back(msg.substr(i, 4+ length));
                    continue;
                }
                else {
                    return msgs;
                }
            }
            return msgs;
        }
    }
    return msgs;
}


bool MsgTyper::ParseMsg_map(Peer& myself, Peer& peer) {
    if( processors.empty() ) {
        InitMsgProcessors();
    }
    std::vector<ustring> msgs( std::move( SplitMsg(myself, peer) ) );

    std::map<int, boost::function<bool(Peer&,Peer&,ustring)>> processor;
    for( auto& msg : msgs ) {
        if( processors.find(static_cast<int>(msg[4])) != processors.end() ) {
            processors[static_cast<int>(msg[4])](myself, peer, msg);
        }
    }
}



bool MsgTyper::ParseMsg(Peer& myself, Peer& peer) {
    ustring words;
    words += UCHAR(19);
    words += reinterpret_cast<const unsigned char*>("BitTorrent protocol");

    std::vector<ustring> msgs( std::move( SplitMsg(myself, peer) ) );

    for( auto& msg: msgs) {
        if( words == msg.substr(0, 20)) {
            ProcessHandShakeMsg(myself, peer, msg);

        } else if( msg[ 4 ] ==  MsgNum::kChoke) {
            ProcessChokeMsg(myself, peer, msg);

        } else if( msg[ 4 ] == MsgNum::kUnchoke ) {
            ProcessUnchokeMsg(myself, peer, msg);

        } else if( msg[ 4 ] == MsgNum::kUnchoke ) {
            ProcessUnchokeMsg(myself, peer, msg);

        } else if( msg[ 4 ] == MsgNum::kInterested ) {
            ProcessInterestedMsg(myself, peer, msg);

        } else if( msg[ 4 ] == MsgNum::kUninterested ) {
            ProcessUninterestedMsg(myself, peer, msg);

        } else if( msg[  4 ] == MsgNum::kHave ) {
            ProcessHaveMsg(myself, peer, msg);

        } else if( msg[ 4 ] == MsgNum::kBitfield ) {
            ProcessBitfieldMsg(myself, peer, msg);

        } else if( msg[ 4 ] == MsgNum::kRequest ) {
            ProcessRequestMsg(myself, peer, msg);

        } else if( msg[ 4 ] == MsgNum::kPiece ) {
            ProcessPieceMsg(myself, peer, msg);

        } else if( msg[ 4 ] == MsgNum::kCancel ) {
            ProcessCancelMsg(myself, peer, msg);

        } else if( msg[ 4 ] == MsgNum::kPort ) {
            ProcessPieceMsg(myself, peer, msg);
        } else {
            continue;
        }
    } // end for

    return 0;
}



bool MsgTyper::ProcessHandShakeMsg(Peer& myself, Peer& peer, const ustring& str) {

    return true;
}


bool MsgTyper::ProcessKeepAliveMsg(Peer& myself, Peer& peer, const ustring& str) {

    return true;
}


bool MsgTyper::ProcessChokeMsg(Peer& myself, Peer& peer, const ustring& str) {

    return true;
}


bool MsgTyper::ProcessUnchokeMsg(Peer& myself, Peer& peer, const ustring& str) {

    return true;
}


bool MsgTyper::ProcessInterestedMsg(Peer& myself, Peer& peer, const ustring& str) {

    return true;
}


bool MsgTyper::ProcessUninterestedMsg(Peer& myself, Peer& peer, const ustring& str) {

    return true;
}


bool MsgTyper::ProcessHaveMsg(Peer& myself, Peer& peer, const ustring& str) {

    return true;
}


bool MsgTyper::ProcessBitfieldMsg(Peer& myself, Peer& peer, const ustring& str) {

    return true;
}


bool MsgTyper::ProcessRequestMsg(Peer& myself, Peer& peer, const ustring& str) {

    return true;
}


bool MsgTyper::ProcessPieceMsg(Peer& myself, Peer& peer, const ustring& str) {

    return true;
}


bool MsgTyper::ProcessCancelMsg(Peer& myself, Peer& peer, const ustring& str) {

    return true;
}


bool MsgTyper::ProcessPortMsg(Peer& myself, Peer& peer, const ustring& str) {

    return true;
}




















