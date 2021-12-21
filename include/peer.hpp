#ifndef BITUSK_SRC_PEER_H__
#define BITUSK_SRC_PEER_H__
#include <iostream>
#include <queue>
#include <list>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <string>

#include "bitfield.hpp"


using namespace boost::asio;

struct RequestPiece {
    RequestPiece() = delete;
    RequestPiece(const RequestPiece& rp) = default;
    RequestPiece& operator=(const RequestPiece& rp) = default;
    ~RequestPiece() = default;

    RequestPiece(size_t i, size_t so, size_t len);

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


class Peer {
public:
    Peer() = default;
    //Peer()

    typedef void(*ProcessorType)(Peer& self, Peer& peer);
    //ip::tcp::socket sock; // using socket ptr;
    //ip::tcp::endpoint ep;
    ProcessorType processor;
    int state;  // 有可能弃用了。
    bool am_choking;
    bool am_interested;
    bool peer_choking;
    bool peer_interested;

    BitMap bitmap;
    
    // buffer design
    // TODO
    enum {kMaxBufferSize = 2048};
    std::basic_string<unsigned char> read_buffer;
    std::basic_string<unsigned char> write_buffer;

    std::queue<RequestPiece> request_queue; //我向peer请求的。
    // 那如何知道我要cancel 哪个Piece？？

    std::queue<RequestPiece> requested_queue; //Peer向我请求的piece

    // Speed Counting
    // TODO

    // Info_hash
    std::basic_string<unsigned char> info_hash;
    std::basic_string<unsigned char> peer_id;
public:
    // provide some easy way to deal with this struct
    Peer& SetState(int st);

    bool CheckConnection();
};


class PeersManager: boost::noncopyable{
public:
    PeersManager() = default;

private:
    std::list<Peer> peers_;
};

#endif  // BITUSK_SRC_PEER_H__