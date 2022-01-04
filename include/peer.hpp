#ifndef BITUSK_SRC_PEER_H__
#define BITUSK_SRC_PEER_H__
#include <cstddef>
#include <iostream>
#include <string>
#include <iomanip>
#include <queue>
#include <list>
#include <atomic>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include "bitfield.hpp"


using namespace boost::asio;

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


struct Peer {
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
    SpeedCounter scounter;

    // Info_hash
    std::basic_string<unsigned char> info_hash;
    std::basic_string<unsigned char> peer_id;

public:
   Peer& SetState(int st);

    bool CheckConnection();

    const std::string GetInfoHash() {
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

    const std::string GetPeerId() {
        std::string result;
        if( peer_id.size() != 20 ) {
            return result;
        }
        result.assign(reinterpret_cast<const char*>(peer_id.c_str()), peer_id.size());
        return result;
    }
};


class PeersManager{
public:
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

private:

    PeersManager() = default;
    std::vector<boost::shared_ptr<Peer>> peers_;
    Peer myself_;
};




#endif  // BITUSK_SRC_PEER_H__