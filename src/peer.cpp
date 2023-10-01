#include "peer.hpp"
#include "message.hpp"

#include <atomic>
#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/function.hpp>
#include <boost/smart_ptr/make_shared_array.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <mutex>

// RequestPiece::RequestPiece(size_t i, size_t so, size_t len): index(i), slice_offset(so), length(len) {}

// class PeersManager
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
#else
std::atomic<PeersManager*> PeersManager::m_instance;
std::mutex PeersManager::m_mtx;
#endif  //

PeersManager* PeersManager::InitInstance(boost::asio::io_context& ioc) {
  PeersManager* tmp = m_instance.load(std::memory_order_relaxed);
  std::atomic_thread_fence(std::memory_order_acquire);
  if (nullptr == tmp) {
    std::lock_guard<std::mutex> lok(m_mtx);
    if (nullptr == tmp) {
      tmp = new PeersManager(ioc);
      std::atomic_thread_fence(std::memory_order_acquire);
      m_instance.store(tmp, std::memory_order_relaxed);
    }
  }

  return tmp;
}

PeersManager* PeersManager::Instance() {
  return m_instance.load(std::memory_order_relaxed);
}

bool IOcall(Peer& myself, Peer& peer) {
  return peer.msg_handler(myself, peer);
}

/*

bool Initial(Peer &myself, Peer& peer) {
    // 判断是接受连接的还是主动连接别人
    // 主动连接别人 生成消息然后发送
    // 被动连接，解析消息，并生成消息发送出去
    peer.write_buffer_str.clear();
    peer.msg_handler(boost::ref(myself), boost::ref(peer));
    peer.processor = HalfShaked;
}



bool HalfShaked(Peer &myself, Peer &peer) {
    // parse recv shake msg
    if (peer.msg_handler(boost::ref(myself), boost::ref(peer))) {
        peer.processor = HandShaked;
    }
}


bool HalfShakedRead(Peer &myself, Peer &peer) {
    // parse recv shake msg
    if (peer.msg_handler(boost::ref(myself), boost::ref(peer))) {
        peer.processor = HandShaked;
    }
}


bool HandShaked(Peer &myself, Peer &peer) {
    if( peer.msg_handler(boost::ref(myself), boost::ref(peer)) ) {
        peer.processor = SendBitfield;
    }
}


bool SendBitfield(Peer &myself, Peer &peer) {
    if( peer.msg_handler(boost::ref(myself), boost::ref(peer))) {
        peer.processor = Data;
        peer.data_exchange_processor = Data01;
    }
}


bool RecvBitfield(Peer &myself, Peer &peer) {
    if( peer.msg_handler(boost::ref(myself), boost::ref(peer))) {
        peer.processor = Data;
        peer.data_exchange_processor = Data01;
    }
}


bool Data(Peer &myself, Peer &peer) {
    peer.data_exchange_processor(myself, peer);
    peer.data_exchange_processor = Data01;
}

*/

bool Closing(Peer& myself, Peer& peer) {}

bool Data01(Peer& myself, Peer&) {
  //
}

bool Data00(Peer& myself, Peer&) {}

bool Data11(Peer& myself, Peer&) {}

bool Data10(Peer& myself, Peer&) {}
