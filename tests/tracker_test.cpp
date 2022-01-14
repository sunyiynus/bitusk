#include "metafileparser.hpp"
#include "catch.hpp"
#include "tracker.hpp"

#include <boost/asio/io_context.hpp>
#include <cstddef>
#include <iostream>
#include <string>

boost::asio::io_context ioserv;


TEST_CASE("tracker connection testing", "[class TrackersManager]") {
    using namespace bitusk;

    auto metafile = bitusk::MetafileObject::readMetaStrFromFile("./test-bt.torrent");
    auto files = metafile->getFiles();
    REQUIRE( files.size() == 2);
    PeersManager* pm = PeersManager::Instance();
    Peer& myself = pm->GetMyself();
    myself.info_hash = bitusk::MetafileObject::CaculateSha1(metafile->getString("info"));
    //myself.info_hash = bitusk::StringToUstring(src);
    myself.peer_id = bitusk::StringToUstring(metafile->getString("peer id"));

    auto announce = metafile->getList("announce-list");
    REQUIRE( announce.size() > 0 );
    long long piecel = metafile->getInt("piece length");
    std::cout << "piece length : " << piecel << std::endl;
    //myself.scounter.file_total_size = 
    TrackersManager::ptr tmanager = TrackersManager::Start(announce, ioserv);
    //tmanager->Run();
    //tmanager.ConnectTrackers(announce);
}


#define INIT_PEERSMANAGER(x) PeersManager* x = PeersManager::Instance()

TEST_CASE("GenerateRequestMsg test", "[function GenerateRequestMsg]") {
    using namespace bitusk;

    auto metafile = bitusk::MetafileObject::readMetaStrFromFile("./test-bt.torrent");


    auto files = metafile->getFiles();

    size_t total  = 0;
    for( auto& f: files) {
        total += f.length; 
    }

    

    PeersManager* pm = PeersManager::Instance();
    Peer& myself = pm->GetMyself();
    
    myself.scounter.file_total_size = total;

    myself.info_hash = bitusk::MetafileObject::CaculateSha1(metafile->getString("info"));
    //myself.info_hash = bitusk::StringToUstring(src);
    myself.peer_id = bitusk::StringToUstring(metafile->getString("peer id"));
    std::ostringstream ostr;
    Peer& peer = pm->GetMyself();
    REQUIRE( peer.info_hash.size() > 10 );
    REQUIRE( peer.peer_id.size() > 10 );
    ostr << "GET /announce?info_hash="<< peer.GetInfoHash() 
        << "&peer_id=" << peer.GetPeerId()
        << "&port=" << "100"
        << "&uploaded=" << peer.scounter.uploads
        << "&downloaded=" << peer.scounter.downloads
        << "$left=" << (peer.scounter.file_total_size - peer.scounter.downloads)
        << "&event=" << "started"
        //<< "&key=" << key
        << "&compact=1"
        //<< "&numwant=" << num_want
        << " HTTP/1.0\r\n"
        << "Host: " << "tracker" << "\r\n"
        << "User-Agent: Bittorent\r\b"
        << "Accept:*/*\r\n"
        << "Accept-Encoding: gzip\r\n"
        << "Connection: closed\r\n\r\n";
    std::string std_result = ostr.str();
    std::string result = GenerateRequestMsg("started", "100", "tracker");
    REQUIRE(result == std_result);

    const unsigned char tmp[21]  = "\x12\x34\x56\x78\x9a\xbc\xde\xf1\x23\x45\x67\x89\xab\xcd\xef\x12\x34\x56\x78\x9a";
    MsgTyper::ustring str (tmp);
    char buffer[100];
    http_encode(str.c_str(), str.size(), buffer, 100);
    std::string resstr (buffer);
    REQUIRE(resstr == "%124Vx%9A%BC%DE%F1%23Eg%89%AB%CD%EF%124Vx%9A");

}