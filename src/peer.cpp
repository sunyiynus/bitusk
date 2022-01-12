#include "peer.hpp"
#include "message.hpp"

#include <atomic>
#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <mutex>
#include <boost/smart_ptr/make_shared_array.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/function.hpp>


// RequestPiece::RequestPiece(size_t i, size_t so, size_t len): index(i), slice_offset(so), length(len) {}





// class PeersManager
#if ((defined(_MSVC_LANG) && _MSVC_LANG >=  201703L ) || __cplusplus >=  201703L) 
#else
std::atomic<PeersManager*> PeersManager::m_instance;
std::mutex PeersManager::m_mtx;
#endif //


PeersManager* PeersManager::InitInstance(boost::asio::io_context& ioc) {
    PeersManager* tmp = m_instance.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if( nullptr ==  tmp ) {
        std::lock_guard<std::mutex> lok(m_mtx);
        if( nullptr == tmp ) {
            tmp = new PeersManager(ioc);
            std::atomic_thread_fence(std::memory_order_acquire);
            m_instance.store(tmp, std::memory_order_relaxed );
        }
    }

    return tmp;
}


PeersManager* PeersManager::GetInstance() {
    return m_instance.load(std::memory_order_relaxed);
}



bool Initial(Peer &myself, Peer& peer) {
    peer.write_buffer_str.clear();
    peer.write_buffer_str = MsgTyper::CreateHandShakedMsg(myself, peer);
    peer.processor = HalfShaked;
}


bool HalfShaked(Peer &myself, Peer &peer) {
    std::string msg (peer.read_buffer);
    // parse recv shake msg
    if (MsgTyper::ParseMsg(myself, peer)) {
        peer.processor = HandShaked;
    }
}


bool HandShaked(Peer &myself, Peer &peer) {
    peer.write_buffer_str = MsgTyper::CreateBitfieldMsg(myself, peer);
    peer.processor = SendBitfield;
}


bool SendBitfield(Peer &myself, Peer &peer) {
    if( MsgTyper::ParseMsg(myself, peer)) {
        peer.processor = Data;
        peer.data_exchange_processor = Data01;
    }
}


bool Data(Peer &myself, Peer &peer) {
    peer.data_exchange_processor(myself, peer);
    peer.data_exchange_processor = Data01;
}

bool Closing(Peer &myself, Peer &peer) {

}

bool Data01(Peer &myself, Peer &) {
    //
}

bool Data00(Peer &myself, Peer &) {

}

bool Data11(Peer &myself, Peer &) {

}

bool Data10(Peer &myself, Peer &) {

}