#ifndef BITUSK_SRC_PEER_H__
#define BITUSK_SRC_PEER_H__

#include <boost/asio/io_context.hpp>
#include <boost/smart_ptr/make_shared_array.hpp>
#include <cstddef>
#include <iostream>
#include <string>
#include <iomanip>
#include <queue>
#include <list>
#include <atomic>

#include <boost/system/error_code.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>

#include "bitfield.hpp"
#include "message.hpp"
#include "basic.hpp"
#include "log.hpp"

using namespace boost::asio;


typedef boost::shared_ptr<ip::tcp::socket> SocketPtr;


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
    double DownloadSpeed();
    double UploadSpeed();
};






struct Peer {
public:

    typedef boost::shared_ptr<Peer> Ptr;
public:
    Peer() = default;
    Peer(const Peer& p) = default;
    Peer& operator=(const Peer&) = default;

    //Peer()

    boost::function<bool(Peer& myself, Peer& peer)> processor;
    boost::function<bool(Peer& myself, Peer& peer)> data_exchange_processor;
    boost::function<bool(Peer& myself, Peer& peer)> msg_handler;
    SocketPtr socket;

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
    const Peer Clone() const;
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

inline const Peer Peer::Clone() const {
    Peer peer;
    peer.info_hash = info_hash;
    peer.am_choking = false;
    peer.am_interested = false;
    peer.peer_choking = true;
    peer.peer_interested = false;
    peer.scounter.downloads = 0;
    peer.scounter.file_total_size = scounter.file_total_size;
    peer.scounter.uploads = 0;
    return peer;
}



bool Initial(Peer& myself, Peer& peer);
bool HalfShaked(Peer& myself, Peer& peer);
bool HandShaked(Peer& myself, Peer& peer);
bool SendBitfield(Peer& myself, Peer& peer);
bool Data(Peer& myself, Peer& peer);
bool Closing(Peer& myself, Peer& peer);


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
    static PeersManager* Instance();
#if ((defined(_MSVC_LANG) && _MSVC_LANG >=  201703L ) || __cplusplus >=  201703L) 
    inline static std::atomic<PeersManager*> m_instance;
    inline static std::mutex m_mtx;
#else
    static std::atomic<PeersManager*> m_instance;
    static std::mutex m_mtx;
#endif //

public:
    Peer& GetMyself();
    std::vector<boost::shared_ptr<Peer>> GetPeers();

    bool AddPeers(const std::vector<ip::tcp::endpoint>& eps) {
        for( const auto& ep: eps) {
            AddPeer(ep);
        }
    }

    bool AddPeer(const ip::tcp::endpoint& ep) {
        ConnectPeer(ep);
    }

public:
    // network io

    void ConnectPeer(const ip::tcp::endpoint& ep) {
        LOGGER(logger);
        if( IsPeerInhere(ep) ) {
            return;
        }
        Peer::Ptr peer = boost::make_shared<Peer>( myself_.Clone());
        peer->ep = ep;
        SocketPtr socket = boost::make_shared<ip::tcp::socket>(ioc_);
        peer->socket = socket;
        peer->processor = Initial;
        peer->msg_handler = MsgTyper::CreateMsg;
        logger.Debug() << " this connectting peer " << ep.address().to_string() << ":" <<ep.port();
        socket->async_connect(ep, MEM_FN2(OnConnection, peer, _1));
    }


    void OnConnection(Peer::Ptr peerptr, const error_code& er) {
        LOGGER(logger);
        if( er ) {
            std::cout << peerptr->ep.address().to_string() << " connection failure !" << std::endl;
            unready_peers_.push_back(peerptr);
            return ;
            // write to some;
        }
        peers_.push_back(peerptr);
        logger.Debug() << "Connectting peer : " << peerptr->ep.address().to_string() << "Successful." ;
        
        DoWrite(peerptr);
    }

    void DoWrite(Peer::Ptr peerptr) {

        peerptr->processor(myself_, *peerptr);
        async_write(*(peerptr->socket), buffer(peerptr->write_buffer_str),
                            MEM_FN2(OnWrite, peerptr, _1));
    }


    void OnWrite(Peer::Ptr peerptr, const error_code& er) {
        if( er ){
            // error handle
        }

        DoRead(peerptr);
    }

    void DoRead(Peer::Ptr peerptr) {
        async_read(*(peerptr->socket), buffer(peerptr->read_buffer, Peer::MaxBufferSize),
                    MEM_FN3(OnRead, peerptr, _1, _2));
    }

    void OnRead(Peer::Ptr peerptr, const error_code& er, size_t bytes) {
        if( er ) {
            if( bytes == 0 ) {
                DoWrite(peerptr);
            }
        }

        ProcessMsg(peerptr);
        DoWrite(peerptr);
    }

    void ProcessMsg(Peer::Ptr peerptr) {
        peerptr->processor(myself_, *peerptr);
    }

    void StartRecvPeerConnection() {
        boost::shared_ptr<Peer> ptr = boost::make_shared<Peer>(myself_.Clone());
        ptr->socket = boost::make_shared<ip::tcp::socket>(ioc_);
        acceptor->async_accept(*(ptr->socket), MEM_FN2(RecvPeerConnect, ptr, _1));
    }


    void RecvPeerConnect(Peer::Ptr peerptr, const error_code& er) {
        LOGGER(logger);
        if( er ) {
            logger.Error()<< "accepte failure. \n";
            StartRecvPeerConnection();
            return;
        }
        logger.Debug() << "Accepte " << peerptr->socket->remote_endpoint().address().to_string()
                        <<":" << peerptr->socket->remote_endpoint().port() << "connection! \n";
        peerptr->processor = Initial;
        peerptr->msg_handler = MsgTyper::ParseMsg;
        peers_.push_back(peerptr);
        DoRead(peerptr);

        boost::shared_ptr<Peer> ptr = boost::make_shared<Peer>(myself_.Clone());
        ptr->socket = boost::make_shared<ip::tcp::socket>(ioc_);
        acceptor->async_accept(*(ptr->socket), MEM_FN2(RecvPeerConnect, ptr, _1));


    }


public:

    bool IsPeerInhere(const ip::tcp::endpoint& ep) {
        auto pred = [&](auto& el) { return el->ep == ep;};
        auto result = std::find_if(peers_.begin(), peers_.end(), pred);
        if( result != peers_.end() ) {
            return true;
        }

        auto result1 = std::find_if( unready_peers_.begin(), unready_peers_.end(), pred);
        if( result1 != unready_peers_.end() ) {
            return true;
        }

        return false;
    }

#ifdef DEBUG_MACRO
    void SetAcceptor() {
        acceptor = boost::make_shared<ip::tcp::acceptor>(ioc_,ip::tcp::endpoint(ip::tcp::v4(), 6969) );
    }
#else

#endif

private:

    PeersManager() = delete;
#ifdef DEBUG_MACRO

    PeersManager(io_context& ioc): ioc_(ioc){}
#else
    PeersManager(io_context& ioc): ioc_(ioc), 
                acceptor(boost::make_shared<ip::tcp::acceptor>((ioc, ip::tcp::endpoint(ip::tcp::v4(), 6969)))) {}
#endif
    std::list<boost::shared_ptr<Peer>> peers_;
    std::list<boost::shared_ptr<Peer>> unready_peers_;
    Peer myself_;
    io_context& ioc_;
    boost::shared_ptr<ip::tcp::acceptor> acceptor;
};




inline Peer& PeersManager::GetMyself() {
    return myself_;
}


inline std::vector<boost::shared_ptr<Peer>> PeersManager::GetPeers() {
    return std::vector<boost::shared_ptr<Peer>>(peers_.begin(), peers_.end());
}





#endif  // BITUSK_SRC_PEER_H__