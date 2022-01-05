#ifndef BITUSK_SRC_PEER_H__
#define BITUSK_SRC_PEER_H__
#include <boost/asio/io_context.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <iostream>
#include <string>
#include <iomanip>
#include <queue>
#include <list>
#include <atomic>

#include <boost/asio/ip/tcp.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>

#include "bitfield.hpp"


using namespace boost::asio;


#define MEM_FN(x)       boost::bind(&SelfType::x, shared_from_this())
#define MEM_FN1(x,y)    boost::bind(&SelfType::x, shared_from_this(), y)
#define MEM_FN2(x,y,z)  boost::bind(&SelfType::x, shared_from_this(), y,z)
//#define MEM_FN2(x,y,z)  boost::bind(&SelfType::x, this, y,z)
#define MEM_FN3(x,y,z,m)  boost::bind(&SelfType::x, shared_from_this(), y, z, m)
#define MEM_FN4(x,y,z,m,p)  boost::bind(&SelfType::x, shared_from_this(), y, z, m, p)



struct RequestPiece {
    RequestPiece() = default;
    RequestPiece(const RequestPiece& rp) = default;
    RequestPiece& operator=(const RequestPiece& rp) = default;
    ~RequestPiece() = default;

    RequestPiece(size_t i, size_t so, size_t len):index(i), slice_offset(so), length(len) {}

    size_t index;
    size_t slice_offset;
    size_t length;
};

namespace PeerState{
    constexpr int kUninitial = -2;
    constexpr int kInitial = -1;
    constexpr int kHalfShaked = 0;
    constexpr int kHandShaked = 1;
    constexpr int kSendBitField = 2;
    constexpr int kRecvBitField = 3;
    constexpr int kDataExchange = 4;
    constexpr int kClosing = 5;
};




class SpeedCounter{
public:
    size_t downloads;
    size_t uploads;
    size_t file_total_size;
public:
    void Start();
};




class AbsPeer{
public:
    virtual void InitSocket() = 0;
/*
    void SetState();
    void WriteBuffer();
    void ReadBuffer();
    void GetBitmap() const;
    void GetBitmap();
    void CountingDownload();
    void CountingUpload();
    void Timing();
    void Retiming();

    */
};





typedef boost::shared_ptr<ip::tcp::socket> SocketPtr;



struct Peer {
public:

    typedef boost::shared_ptr<Peer> Ptr;
public:
    Peer() = default;
    //Peer()

    boost::function<bool(Peer& myself, Peer& peer)> processor;
    boost::function<bool(Peer& myself, Peer& peer)> data_exchange_processor;
    boost::shared_ptr<SocketPtr> socket;

    int state;  // 有可能弃用了。
    bool am_choking;
    bool am_interested;
    bool peer_choking;
    bool peer_interested;

    BitMap bitmap;

    ip::tcp::endpoint ep;
    
    // buffer design
    // TODO
    enum {MaxBufferSize = 1024 * 10};
    //std::basic_string<unsigned char> read_buffer;
    std::string write_buffer_str;

    char read_buffer[MaxBufferSize];
    char write_buffer[MaxBufferSize];

    std::queue<RequestPiece> request_queue; //我向peer请求的。
    // 那如何知道我要cancel 哪个Piece？？

    std::queue<RequestPiece> requested_queue; //Peer向我请求的piece

    // Speed Counting
    SpeedCounter scounter;

    // Info_hash
    std::basic_string<unsigned char> info_hash;
    std::basic_string<unsigned char> peer_id;

public:
    Peer& SetState(int st);
    bool CheckConnection();
    const std::string GetInfoHash();
    const std::string GetPeerId();
};



inline Peer& Peer::SetState(int st) {
    state = st;
    return *this;
}


inline const std::string Peer::GetInfoHash() {
    std::string result;
    if( info_hash.size() != 20 ) {
        return result;
    }
    if( info_hash.size() == 20 ) {
        std::basic_ostringstream<char> buf;
        const unsigned int* digest = reinterpret_cast<const unsigned int*>(info_hash.c_str());
        for( int i = 0; i < 5; ++i) {
            buf << std::hex << std::setfill('0') << std::setw(8) << digest[i];
        }
        return buf.str();
    }
}


inline const std::string Peer::GetPeerId() {
    std::string result;
    if( peer_id.size() != 20 ) {
        return result;
    }
    result.assign(reinterpret_cast<const char*>(peer_id.c_str()), peer_id.size());
    return result;
}



bool Initial(Peer& myself, Peer&);
bool HalfShaked(Peer& myself, Peer&);
bool HandShaked(Peer& myself, Peer&);
bool SendBitfield(Peer& myself, Peer&);
bool Data(Peer& myself, Peer&);
bool Closing(Peer& myself, Peer&);


//  peer_interested    am_choking
//  0                  1
bool Data01(Peer& myself, Peer&);
bool Data00(Peer& myself, Peer&);
bool Data11(Peer& myself, Peer&);
bool Data10(Peer& myself, Peer&);


class PeersManager: public boost::enable_shared_from_this<PeersManager>{
public:
    typedef PeersManager SelfType;
    typedef boost::system::error_code error_code;

public:
    static PeersManager* InitInstance(io_context& ioc);
    static PeersManager* GetInstance();
#if ((defined(_MSVC_LANG) && _MSVC_LANG >=  201703L ) || __cplusplus >=  201703L) 
    inline static std::atomic<PeersManager*> m_instance;
    inline static std::mutex m_mtx;
#else
    static std::atomic<PeersManager*> m_instance;
    static std::mutex m_mtx;
#endif //

public:
    Peer& GetMyself();
    std::vector<boost::shared_ptr<Peer>>& GetPeers();
    bool AddPeer(const ip::tcp::endpoint& ep);

public:
    // network io

    void ConnectingPeer(const ip::tcp::endpoint& ep) {
        Peer::Ptr peer = boost::make_shared<Peer>();
        peer->ep = ep;
        peer->socket = boost::make_shared<ip::tcp::socket>(ioc_);
        peer->processor = Initial;
        async_connect(*(peer->socket), ep, MEM_FN2(DoWrite,peer,_1))
    }

    void DoWrite(Peer::Ptr peerptr, const error_code& er) {
        if( er ) return;
        peerptr->processor(myself_, *peerptr);
        peerptr->socket->async_write_some(buffer(peerptr->write_buffer_str),
                            MEM_FN(t))
    }

private:

    PeersManager() = delete;
    std::list<boost::shared_ptr<Peer>> peers_;
    std::queue<boost::shared_ptr<Peer>> unready_peers_;
    Peer myself_;
    io_context& ioc_;
};




inline Peer& PeersManager::GetMyself() {
    return myself_;
}


inline std::vector<boost::shared_ptr<Peer>>& PeersManager::GetPeers() {
    return peers_;
}





#endif  // BITUSK_SRC_PEER_H__