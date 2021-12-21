#ifndef BITUSK_SRC_TRACKER_H__
#define BITUSK_SRC_TRACKER_H__

#include "metafileparser.hpp"
#include "peer.hpp"

#include <boost/noncopyable.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/uuid/detail/sha1.hpp>

#include <list>
#include <memory>

using namespace bitusk;

typedef boost::system::error_code ErrorCodeType;
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;

class Tracker: boost::noncopyable {
public:

    std::string url;
    int port;
    boost::asio::ip::tcp::endpoint ep;
    SocketPtr socketptr;

};


class TrackersManager : boost::noncopyable , 
                              std::enable_shared_from_this<TrackersManager>{
public:
    typedef boost::system::error_code error_code;
    typedef boost::shared_ptr<TrackersManager> ptr;
    TrackersManager() = default;
    TrackersManager(MetafileObject* metafile, std::list<Peer>* peers);

    inline static ptr Start(MetafileObject* metafile, std::list<Peer>* peers) {
        TrackersManager::ptr new_(new TrackersManager(metafile, peers));
        new_->UpdatePeers();
        return new_;
    }

    void UpdatePeers();
    void GetTrackerUrls(MetafileObject& metafileobject);

    std::shared_ptr<TrackersManager> GetPtr();
    void SetPeersManager(PeersManager* peersmanager);


    // Connecting Tracker
    void ConnectingTracker() {
        for (auto& tracker: trackers_) {
            tracker.socketptr->async_connect(tracker.ep, 
                                boost::bind(&TrackersManager::CommunicateWithtraker,
                                    shared_from_this(), std::ref(tracker), _1));
        }
    }


    void CommunicateWithtraker(Tracker& tracker, error_code& err) {  // on_connection
        if ( !err ) DoWrite(err);
    }

    void DoWrite(Tracker& tracker, error_code& err) {
        //tracker.socketptr->async_read( std::bind)
    }

    void OnWrite(error_code& err, size_t bytes) {
        DoRead(err);

    }

    void DoRead(error_code& err) {

    }


    void OnRead(error_code& err) {
        
    }

    int CreateRequest(char* request) {
        char encoded_info_hash[100];
        char encoded_peer_id[100];
        int key;
        char tracker_name[128];

        std::ostringstream ostr;
        ostr << "GET /announce?info_hash="<< encoded_indo_hash 
            << "&peer_id=" << encoded_peer_id
            << "&port=" << port
            << "&uploaded=" << upload
            << "&downloaded=" << download
            << "$left=" << left
            << "&event=started"
            << "&key=" << key
            << "&compact=1"
            << "&numwant=" << num_want
            << " HTTP/1.0\r\n"
            << "Host: " << tracker_name << "\r\n"
            << "User-Agent: Bittorent\r\b"
            << "Accept:*/*\r\n"
            << "Accept-Encoding: gzip\r\n"
            << "Connection: closed\r\n\r\n";
#ifdef DEBUG
        std::out << ostr.str() << std::endl;
#endif

        std::string tmprequest (ostr.str());
        std::copy(tmprequest.begin(), tmprequest.end(), request);

    }
private:
    std::list<Tracker> trackers_;

    boost::asio::io_service ioserver_;

#ifdef GLOBAL_PEER
    void SetPeersList(std::list<Peer>* list);
    std::list<Peer>* peerslist_;
#endif

    PeersManager* mpeer;

};


#endif
