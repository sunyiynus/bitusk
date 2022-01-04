#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iterator>
#include <sstream>
#include <stdexcept>

#include "message.hpp"

// 另外一种设计，比如

AbsMessageGenerator* MsgGeneratorFactory::GetMsgGenerator(Peer& peer, int msg_type) {

    switch (msg_type) {
    case MsgNum::kHandShake:
        return new HandShakeMsg();
        break;
    case MsgNum::kKeepAlive:
        return new KeepAliveMsg();
        break;
    case MsgNum::kChoke:
        return new ChokedMsg();
        break;
    case MsgNum::kUnchoke:
        return new UnChockdMsg();
        break;
    case MsgNum::kInterested:
        return new InterestedMsg();
        break;
    case MsgNum::kUnInterested:
        return new UnInterestedMsg();
        break;
    case MsgNum::kHave:
        return new HaveMsg();
        break;
    case MsgNum::kBitfield:
        return new BitfieldMsg();
        break;
    case MsgNum::kRequest:
        return new RequestMsg();
        break;
    case MsgNum::kPiece:
        return new PieceMsg();
        break;
    case MsgNum::kCancel:
        return new CancelMsg();
        break;
    default:
        return nullptr;
    }

}


AbsMessageParser* MsgParserFactory::GetMsgParser(Peer &peer) {
/*
    switch (CharToInt(peer.read_buffer)) {
        case MsgNum::kCancel:
            return new ProcessCancelMsg();
            break;
        case MsgNum::kCancel:
            return new ProcessCancelMsg();
            break;
        case MsgNum::kCancel:
            return new ProcessCancelMsg();
            break;
        case MsgNum::kCancel:
            return new ProcessCancelMsg();
            break;
        case MsgNum::kCancel:
            return new ProcessCancelMsg();
            break;
        case MsgNum::kCancel:
            return new ProcessCancelMsg();
            break;
        case MsgNum::kCancel:
            return new ProcessCancelMsg();
            break;
        case MsgNum::kCancel:
            return new ProcessCancelMsg();
            break;
        case MsgNum::kCancel:
            return new ProcessCancelMsg();
            break;
        case MsgNum::kCancel:
            return new ProcessCancelMsg();
            break;
        case MsgNum::kCancel:
            return new ProcessCancelMsg();
            break;
        default:
            return nullptr;
            break;
    }
    */
}


int MsgTyper::IntToChar(int i, uostringstream& os) {
    unsigned char c [4] = {0};
    IntToChar(i, c);
    os << c[0] << c[1] << c[2] << c[3];
    return 0;
}


int MsgTyper::IntToChar(int i, unsigned char* c) {
    c[3] = (i % 256);
    c[2] = (i - c[3]) / 256 % 256;
    c[1] = (i - c[3] - c[2]*256) / 256 / 256 % 256;
    c[0] = (i - c[3] - c[2]*256 -c[1]*256*256) / 256 / 256 / 256 % 256;
    return 0;
}


int MsgTyper::CharToInt(const unsigned char * c) {
    int i = 0;
    i = c[0] * 256 * 256 * 256
      + c[1] * 256 * 256
      + c[2] * 256
      + c[3];

    return i;
}


const MsgTyper::ustring TrackerRequestGenerator::GenerateMessage(Peer& peer) {
    std::string event;
    switch(items_[1]) {
        case 1:
            event = "started";
            break;
        case 2:
            event = "completed";
            break;
        case 3:
            event = "stopped";
            break;
        default:
            break;
    }

    std::string tracker_name;
    std::ostringstream ostr;
    ostr << "GET /announce?info_hash="<< peer.GetInfoHash() 
        << "&peer_id=" << peer.GetPeerId()
        << "&port=" << items_[0]
        << "&uploaded=" << peer.scounter.uploads
        << "&downloaded=" << peer.scounter.downloads
        << "$left=" << (peer.scounter.file_total_size - peer.scounter.downloads)
        << "&event=" << event
        //<< "&key=" << key
        << "&compact=1"
        //<< "&numwant=" << num_want
        << " HTTP/1.0\r\n"
        << "Host: " << tracker_name_ << "\r\n"
        << "User-Agent: Bittorent\r\b"
        << "Accept:*/*\r\n"
        << "Accept-Encoding: gzip\r\n"
        << "Connection: closed\r\n\r\n";
#ifdef DEBUG
    std::out << msg.str() << std::endl;
#endif
    return msg.str();
}


TrackerRequestGenerator& TrackerRequestGenerator::Set(std::vector<int>& item) {
    assert( item.size() > 1);
    items_ = item;
    return *this;
}


void TrackerRequestGenerator::Set(const std::string& tname) {
    tracker_name_ = tname;
}


const MsgTyper::ustring HandShakeMsg::GenerateMessage(Peer &peer)
{
    std::string name = "BitTorrent protocol";
    unsigned char pstrlen = 19;
    msg << pstrlen;
    msg << reinterpret_cast<const unsigned char*>(name.c_str());
    for(int i = 0; i < 8; ++i) msg.put((unsigned char)'0x00');
           msg << peer.info_hash.c_str();
           msg << peer.peer_id.c_str();
#ifdef SINGLE_MESSAGE
    peer.write_buffer.clear();
    peer.write_buffer = msg.str();
#endif
    return ustring(msg.str());
}


const MsgTyper::ustring KeepAliveMsg::GenerateMessage(Peer& peer)
{
    unsigned char zero = (unsigned char)'0x00';
    msg << zero
        << zero
        << zero
        << zero;
    return ustring(msg.str());
}


const MsgTyper::ustring ChokedMsg::GenerateMessage(Peer& peer)
{
    IntToChar(1, msg );
    msg << (unsigned char)('0x00');
#ifdef SINGLE_MESSAGE
    peer.write_buffer.clear();
    peer.write_buffer = msg.str();
#endif
    return ustring(msg.str());
}


const MsgTyper::ustring UnChockdMsg::GenerateMessage(Peer& peer)
{
    IntToChar(1, msg );
    msg << (unsigned char)'0x01';
#ifdef SINGLE_MESSAGE
    peer.write_buffer.clear();
    peer.write_buffer = msg.str();
#endif
    return ustring(msg.str());
}


const MsgTyper::ustring InterestedMsg::GenerateMessage(Peer& peer)
{
    IntToChar(1, msg );
    msg << (unsigned char)'0x02';
#ifdef SINGLE_MESSAGE
    peer.write_buffer.clear();
    peer.write_buffer = msg.str();
#endif
    return ustring(msg.str());
}


const MsgTyper::ustring UnInterestedMsg::GenerateMessage(Peer& peer)
{
    IntToChar(1, msg );
    msg << (unsigned char)'0x03';
#ifdef SINGLE_MESSAGE
    peer.write_buffer.clear();
    peer.write_buffer = msg.str();
#endif
    return ustring(msg.str());
}


HaveMsg& HaveMsg::Set(std::vector<int> &item)
{
    if( item.empty()) throw std::logic_error("Have message must have piece index.");
    index_ = item[0];
    return *this;
}


const MsgTyper::ustring HaveMsg::GenerateMessage(Peer& peer)
{
    IntToChar(5, msg );
    msg << (unsigned char)'0x04';
    IntToChar(index_, msg );
#ifdef SINGLE_MESSAGE
    peer.write_buffer.clear();
    peer.write_buffer = msg.str();
#endif
    return ustring(msg.str());
}


const MsgTyper::ustring BitfieldMsg::GenerateMessage(Peer& peer)
{
    if( peer.bitmap.Size() == 0)
    {
        return ustring();
    }
    IntToChar(1 + ((peer.bitmap.Size()%8 == 0)? (peer.bitmap.Size() /8): (peer.bitmap.Size()/8 +1)), 
                msg );
    msg << (unsigned char)'0x05';
    ConvertBitmap(peer, msg);
#ifdef SINGLE_MESSAGE
    peer.write_buffer.clear();
    peer.write_buffer = msg.str();
#endif
    return ustring(msg.str());
}

void BitfieldMsg::ConvertBitmap(Peer &peer, uostringstream &os)
{
    BitMap& bitmap = peer.bitmap;
    int charlen = (bitmap.Size() % 8 == 0) ? (bitmap.Size() / 8): (bitmap.Size() / 8 + 1);
    unsigned char* bits = new unsigned char[charlen];

    for(int i = 0; i < bitmap.Size(); ++i) {
        bits[i / 8] |=  1 << (i % 8);
    }
    ustring bitstring (bits, charlen);
    os << bitstring;
    delete [] bits;
}


// TODO 这两消息有点没理清？？ Cancel和request到底要修改那些peer的request queue和requested queue；
const MsgTyper::ustring RequestMsg::GenerateMessage(Peer& peer)
{
    if( peer.request_queue.empty()) return ustring();
    IntToChar(1 + 12, msg );
    msg << (unsigned char)'0x06';

    RequestPiece reqp = peer.request_queue.front();

    IntToChar(reqp.index, msg);
    IntToChar(reqp.slice_offset, msg);
    IntToChar(reqp.length, msg);

    peer.request_queue.pop();

#ifdef SINGLE_MESSAGE
    peer.write_buffer.clear();
    peer.write_buffer = msg.str();
#endif
    return ustring(msg.str());
}


RequestMsg& RequestMsg::Set(std::vector<int>& item) {
    if( item.empty() )
    {
        if( item.empty()) throw std::logic_error("request message must have piece index.");
    }
}


const MsgTyper::ustring PieceMsg::GenerateMessage(Peer& peer)
{
    IntToChar(1 + peer.bitmap.Size(), msg );
    msg << (unsigned char)'0x07';

#ifdef SINGLE_MESSAGE
    peer.write_buffer.clear();
    peer.write_buffer = msg.str();
#endif
    return ustring(msg.str());
}

const MsgTyper::ustring CancelMsg::GenerateMessage(Peer& peer)
{
    IntToChar(1 + 12, msg );
    msg << (unsigned char)'0x08';
}

CancelMsg& CancelMsg::Set(std::vector<int>& item)
{

    return *this;
}


const MsgTyper::ustring PortMsg::GenerateMessage(Peer& peer)
{
    // this feature was used lessly.
}

void ProcessHandShakeMsg::ParseMessage(Peer &peer, const ustring &msg)
{
    if( msg[4] == 0 ){
        return;
    }

    peer.state = PeerState::kHandShaked;
    return;
}


void ProcessKeepAliveMsg::ParseMessage(Peer& peer, const ustring &msg)
{
    // retiming
}


void ProcessChokeMsg::ParseMessage(Peer& peer, const ustring& msg)
{
    // myself choked or peer choked???
    peer.peer_choking = true;
    // 清理 peer的相关数据
}

/*
void ProcessUnchokeMsg::ParseMessage(Peer& peer, const ustring& msg)
{
    // TODO the same with above
    if(peer.state != PeerState::kClosing &&
        peer.peer_choking)
    {
        peer.choking = false;
        if( peer.am_interested)
        {
            CreateReqSliceMsg(peer);
        } 
        else 
        {
            peer.am_interested = is_interested(my_self, peer);
            if( !peer.am_interested)
            {
                CreateReqSliceMsg(peer);
            }
            else
            {
                // some logger;
            }

        }

        // timing or counting TODO
    }
}


void ProcessInterestedMsg:ParseMessage(Peer& peer, const ustring& msg)
{
    // TODO
    // the same question with above
}

void ProcessUninterestedMsg::ParseMessage(Peer& peer, const ustring& msg)
{
    // TODO
}


void ProcessHaveMsg::ParseMessage(Peer& peer, const ustring& msg)
{
    int len = CharToInt(msg.c_str());
    ustring context (msg.cbegin()+5, msg.cbegin() + 5 + len);
    // convert have msg to what i need;
    // 这儿应该和那个piece有关需要更改bitmap

}


void ProcessCancelMsg::ParseMessage(Peer& peer, const ustring& msg)
{
    // TODO
}


void ProcessBitfieldMsg::ParseMessage(Peer& peer, const ustring& msg)
{
    int len = CharToInt(msg.c_str()) -1;
    ustring context (msg.cbegin()+5, msg.cbegin() + 5 + len);
    std::vector<bool> tmp_bits;
    const unsigned char* bits = msg.c_str();
    assert(len * 8 >= peer.bitmap.Size());
    for( int i = 0; i < peer.bitmap.Size(); ++i )
    {
        bool bit = bits[i / 8] & (1<< (i%8));
        tmp_bits.push_back(bit);
    }

    // 给哪个peer

}


void ProcessRequestMsg::ParseMessage(Peer& peer, const ustring& msg)
{
    int index = CharToInt(msg.c_str() + 5);
    int offset = CharToInt(msg.c_str() + 5 + 4);
    int length = CharToInt(msg.c_str() + 5 + 4 + 4);

    peer.requested_queue.emplace(index,offset,length);

}


void ProcessPieceMsg::ParseMessage(Peer& peer, const ustring& msg)
{    
    int len = CharToInt(msg.c_str());
    ustring context (msg.cbegin()+5, msg.cbegin() + 5 + len);
 
    // 放入datacache中
}


*/










