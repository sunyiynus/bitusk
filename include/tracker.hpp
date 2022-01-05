#ifndef BITUSK_SRC_TRACKER_H__
#define BITUSK_SRC_TRACKER_H__

//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

#include "metafileparser.hpp"
#include "peer.hpp"
#include "message.hpp"
#include "bcode.hpp"

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/concept_check.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/exception/exception.hpp>
#include <boost/noncopyable.hpp>
#include <boost/operators.hpp>
#include <boost/smart_ptr/make_shared_array.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/system/system_error.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <list>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <arpa/inet.h>
#include <mutex>


#define  DEBUG

using namespace bitusk;
using namespace boost::asio;
typedef boost::system::error_code ErrorCodeType;
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;
typedef boost::shared_ptr<boost::asio::ip::udp::socket> uSocketPtr;
typedef std::map<std::string,std::string> dictionary;


#define MEM_FN(x)       boost::bind(&SelfType::x, shared_from_this())
#define MEM_FN1(x,y)    boost::bind(&SelfType::x, shared_from_this(), y)
#define MEM_FN2(x,y,z)  boost::bind(&SelfType::x, shared_from_this(), y,z)
//#define MEM_FN2(x,y,z)  boost::bind(&SelfType::x, this, y,z)
#define MEM_FN3(x,y,z,m)  boost::bind(&SelfType::x, shared_from_this(), y, z, m)
#define MEM_FN4(x,y,z,m,p)  boost::bind(&SelfType::x, shared_from_this(), y, z, m, p)


constexpr size_t MAXBUFFERLEN = 1024 * 10;


const std::string GenerateRequestMsg(const std::string& event, const std::string& port, const std::string& tracker_name);
int http_encode(const unsigned char* in, int lenin, char* out, int lenout);
const std::vector<ip::tcp::endpoint> GetPeersFromHttpResponse(const std::string& response);
size_t GetMinIntervalFromHttpResponse(const std::string& str);


//template <typename T>
struct Tracker{
public:
    Tracker() = default;
    Tracker(const Tracker& tck) = default;
    Tracker& operator=(const Tracker& tck) = default;
    Tracker(Tracker&& tck) = default;
    SocketPtr socketptr;
    uSocketPtr usocketptr;

    std::string url;
    std::string port;
    std::string state;
    dictionary records;
    boost::shared_ptr<deadline_timer> timer;
    ip::tcp::endpoint ep;

    char WriteBuffer[MAXBUFFERLEN];
    char ReadBuffer[MAXBUFFERLEN];
};


typedef boost::shared_ptr<Tracker> TrackerPtr;


class TrackersManager :public boost::enable_shared_from_this<TrackersManager>, boost::noncopyable{
public:
    typedef TrackersManager SelfType;
    typedef boost::system::error_code error_code;
    typedef boost::shared_ptr<TrackersManager> ptr;
    TrackersManager() = delete;
    TrackersManager(boost::asio::io_context& ioserv):ioserver_(ioserv) {}
    //TrackersManager(std::list<Peer>* peers) {}

    static ptr Start(const std::vector<std::string>& trackers , boost::asio::io_context& ioserv) {
        TrackersManager::ptr new_ = boost::make_shared<TrackersManager>(ioserv);
        new_->ConnectTrackers(trackers);
        return new_;
    }

    void ConnectTrackers(const std::vector<std::string>& trackers) {
        for( auto& tracker: trackers ) {
            if( tracker.find("udp") != std::string::npos ) {
                // 暂时不支持udp 
                continue;
            } else {
                ConnectTcpTracker(tracker);
            }
        }
    }


    void ConnectTcpTracker(const std::string& tracker) {
        TrackerPtr tck = boost::make_shared<Tracker>();
        tck->url = tracker.substr(0, tracker.find_last_of(":"));
        std::cout << tck->url << std::endl;
        std::string domain = "www." +
            tck->url.substr(tck->url.find("//") + 2, tck->url.size()); 
        tck->port = tracker.substr(tracker.find_last_of(":") + 1,
                 tracker.find_last_of("/") - tracker.find_last_of(":") - 1);
        
        std::cout << domain << " : " << tck->port << std::endl;

        try{
            ip::tcp::resolver resolver(ioserver_);
            ip::tcp::resolver::query query(domain, tck->port);
            ip::tcp::resolver::iterator itr = resolver.resolve(query);
            ip::tcp::endpoint ep = *itr;
            std::cout << ep.address().to_string() << std::endl;
            SocketPtr sok (new ip::tcp::socket(ioserver_));
            tck->socketptr = sok;
            tck->state = "started";
            sok->async_connect(ep, MEM_FN2(OnConnection, tck, _1));
        } catch ( const boost::wrapexcept<boost::system::system_error>& err) {
            std::cout << err.what() << std::endl;
            std::cout << "Resovle failure.." << std::endl;
        }
    }

    void ConnectUdpTracker(const std::string& tracker) {
        TrackerPtr tck = boost::make_shared<Tracker>();
        tck->url = tracker.substr(0, tracker.find_last_of(":"));
        std::cout << tck->url << std::endl;
        std::string domain = "www." +
            tck->url.substr(tck->url.find("//") + 2, tck->url.size()); 
        tck->port = tracker.substr(tracker.find_last_of(":") + 1,
                 tracker.find_last_of("/") - tracker.find_last_of(":") - 1);
        
        std::cout << domain << " : " << tck->port << std::endl;

        try{
            ip::tcp::resolver resolver(ioserver_);
            ip::tcp::resolver::query query(domain, tck->port);
            ip::tcp::resolver::iterator itr = resolver.resolve(query);
            ip::tcp::endpoint ep = *itr;
            std::cout << ep.address().to_string() << std::endl;

            SocketPtr sok (new ip::tcp::socket(ioserver_));
            tck->socketptr = sok;
            tck->state = "started";
            tck->ep = ep;
            sok->async_connect(ep, MEM_FN2(OnConnection, tck, _1));
        } catch ( const boost::wrapexcept<boost::system::system_error>& err) {
            std::cout << err.what() << std::endl;
            std::cout << "Resovle failure.." << std::endl;
        }
    }

    //void Connecting()
    void OnConnection(TrackerPtr tck, const error_code& er) {
        if( !er ) {
            std::cout <<tck->url <<" Connection sucessful" << std::endl;
            trackers_.push_back(tck);
            DoWrite(tck);
        }
    }

    void ShiftToWrite(TrackerPtr tck, const error_code& er) {
        if( !er ) tck->socketptr->async_connect(tck->ep, MEM_FN2(OnConnection, tck, _1));
    }


    void DoWrite(TrackerPtr tck) {
       if( !tck->socketptr.get() ) {
           throw std::logic_error("Do write src not ready..");
       }

       std::string msg = CreateMessage(tck);
       std::cout << "Request to the server : "<< msg << std::endl; 
       tck->socketptr->async_write_some(buffer(msg.c_str(), msg.size()),
            MEM_FN3(OnWrite, tck, _1, _2));

    }

    void OnWrite(TrackerPtr tck,const error_code& err, size_t bytes) {
        if( !err ) {
            std::cout << "Write msg successful !" << std::endl;
            DoRead(tck);
        }
    }

    void DoRead(TrackerPtr tck) {
        async_read(*(tck->socketptr) ,buffer(tck->ReadBuffer,MAXBUFFERLEN), 
        MEM_FN3(OnRead, tck, _1, _2));
    }

    void OnRead(TrackerPtr tck, const error_code& er, size_t bytes) {
        if( !er ) {
            std::cout << "read msg failure .. " << std::endl;
        } 
        //DoWrite(tck);
        ProcessMessage(tck);
    }

    const std::string CreateMessage(TrackerPtr tck) {
        if( tck->state == "started") {
            std::string result =  GenerateRequestMsg(tck->state, tck->port, tck->url);
            return result;
        } else if ( tck->state ==  "completed"){

        } else if ( tck->state == "stopped") {

        }
    }

    void ProcessMessage(TrackerPtr tck) {
        std::string recv_msg (tck->ReadBuffer);
        std::cout << tck->ReadBuffer<< std::endl;
        if( recv_msg.find("400 Invalid Request") != std::string::npos ) {
            std::cout << "the request didn't matching the standard." << std::endl;
            return;
        }

        auto peers = GetPeersFromHttpResponse(recv_msg);
        size_t min_interval = GetMinIntervalFromHttpResponse(recv_msg);
#ifdef DEBUG
        std::cout << "time min interval : " << min_interval << std::endl;
#endif

        tck->socketptr->close();
        tck->timer = boost::make_shared<deadline_timer>(ioserver_,
                        boost::posix_time::seconds( (min_interval > 0)? 
                                    (min_interval + 10):(1000)));
        tck->timer->async_wait(MEM_FN2(ShiftToWrite,tck, _1));

        std::memset(tck->ReadBuffer,0,MAXBUFFERLEN);
    }


private:

    std::list<TrackerPtr> trackers_;

    boost::asio::io_context& ioserver_;

#ifdef GLOBAL_PEER
    void SetPeersList(std::list<Peer>* list);
    std::list<Peer>* peerslist_;
#endif

    size_t event_count;
    

};



const std::string GenerateRequestMsg(const std::string& event, 
    const std::string& port, const std::string& tracker_name) {
    PeersManager* pm = PeersManager::GetInstance();
    Peer& peer = pm->GetMyself();
    std::ostringstream ostr;
    char encoded_info_hash[100];
    char encoded_peer_id[100];
    http_encode(peer.info_hash.c_str(), peer.info_hash.size(), encoded_info_hash, 100);
    http_encode(peer.peer_id.c_str(), peer.peer_id.size(), encoded_peer_id, 100);

    std::string trackern;
    if( tracker_name.find("http://") != std::string::npos ) {
        trackern = tracker_name.substr(tracker_name.find("http://") + 7, tracker_name.size() - 7);
    } else if( tracker_name.find("https://") != std::string::npos ) {
        trackern = tracker_name.substr(tracker_name.find("https://") + 8, tracker_name.size() - 8);
    }
    std::cout << peer.GetInfoHash() << std::endl;

    srand(time(NULL));
    int key = rand()/1000;
    
    ostr << "GET /announce?info_hash="<< encoded_info_hash 
        << "&peer_id=" << encoded_peer_id
        << "&port=" << port
        << "&uploaded=" << peer.scounter.uploads
        << "&downloaded=" << peer.scounter.downloads
        << "&left=" << (peer.scounter.file_total_size - peer.scounter.downloads)
        << "&event=" << event
        << "&key=" << key
        << "&compact=1"
        << "&numwant=" << 200
        << " HTTP/1.0\r\n"
        << "Host: "  <<  trackern << "\r\n"
        << "User-Agent: Bittorrent\r\b"
        << "Accept:*/*\r\n"
        << "Accept-Encoding: gzip\r\n"
        << "Connection: closed\r\n\r\n";

    return ostr.str();
}


const std::vector<ip::tcp::endpoint> GetPeersFromHttpResponse(const std::string& response) {
    std::vector<ip::tcp::endpoint> result;
    if ( response.empty() ) {
        return result;
    }


    // just using c style
    std::string::size_type pos = std::string::npos;
    if ( (pos = response.find("5:peers")) != std::string::npos ) {
        std::string cp_response ( response);
        std::string::iterator itr = cp_response.begin() + pos + 7;
        std::string peers = Bcode::parseStr(itr);
        std::cout << peers << " size of " << peers.size()<< std::endl;
        for( size_t i = 0; i < peers.size() / 6; ++i) {
            size_t offset = i * 6;
            // read port
            char port[2];
            port[0] = response[ offset + 4];
            port[1] = response[ offset + 5];
            unsigned short pint= ntohs(*(unsigned short*) &port[0]);

            // read ip address
            unsigned char addr[4];
            addr[0] = static_cast<unsigned char>(response[ offset + 0]);
            addr[1] = static_cast<unsigned char>(response[ offset + 1]);
            addr[2] = static_cast<unsigned char>(response[ offset + 2]);
            addr[3] = static_cast<unsigned char>(response[ offset + 3]);

            char buff[100];
            sprintf(buff, "%u.%u.%u.%u",addr[0],
                                        addr[1],
                                        addr[2],
                                        addr[3]);

            ip::tcp::endpoint ep (ip::address::from_string(std::string(buff)), pint);
            result.push_back(std::move(ep));
            
        }
    }
#ifdef DEBUG
    for( auto& ep: result ) {
        std::cout << ep.address().to_string() << std::endl;
    }
#endif
    return result;
}


size_t GetMinIntervalFromHttpResponse(const std::string& str) {
    std::string::size_type pos = std::string::npos;
    std::string cp_response (str);
    if( (pos = cp_response.find("12:min interval")) != std::string::npos ){
        std::string::iterator itr = cp_response.begin() + pos + 15;

        return Bcode::parseInt(++itr);
    }
    return 0;
}

int http_encode(const unsigned char* in, int lenin, char* out, int lenout) {
    int i = 0, j = 0;
    char hex_table[17] = "0123456789abcdef";

    if( (lenin != 20) || (lenout <= 90) ) return -1;

    for( i = 0, j = 0; i < 20; i++, j++) {
        if( std::isalpha(in[i]) || std::isdigit(in[i]) )  {
            out[j] = in[i];
        } else {
            out[j] = '%';
            j++;
            out[j] = hex_table[in[i] >> 4];
            j++;
            out[j] = hex_table[in[i] & 0xf];
        }
    }
    out[j] = '\0';

    return 0;
}


#endif
