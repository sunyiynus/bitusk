#include "peer.hpp"

#include <atomic>
#include <boost/asio/connect.hpp>
#include <mutex>
#include <boost/smart_ptr/make_shared_array.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/function.hpp>


// RequestPiece::RequestPiece(size_t i, size_t so, size_t len): index(i), slice_offset(so), length(len) {}



Peer& Peer::SetState(int st) {
    state = st;
    return *this;
}





// class PeersManager
#if ((defined(_MSVC_LANG) && _MSVC_LANG >=  201703L ) || __cplusplus >=  201703L) 
#else
std::atomic<PeersManager*> PeersManager::m_instance;
std::mutex PeersManager::m_mtx;
#endif //


PeersManager* PeersManager::GetInstance() {
    PeersManager* tmp = m_instance.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if( nullptr ==  tmp ) {
        std::lock_guard<std::mutex> lok(m_mtx);
        if( nullptr == tmp ) {
            tmp = new PeersManager();
            std::atomic_thread_fence(std::memory_order_acquire);
            m_instance.store(tmp, std::memory_order_relaxed );
        }
    }

    return tmp;
}


inline Peer& PeersManager::GetMyself() {
    return myself_;
}


inline std::vector<boost::shared_ptr<Peer>>& PeersManager::GetPeers() {
    return peers_;
}
