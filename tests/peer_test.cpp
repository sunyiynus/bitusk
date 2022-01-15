#include <iostream>
#include <sstream>
#include <string>

#include "peer.hpp"
#include "catch.hpp"
#include "metafileparser.hpp"



using namespace bitusk;

TEST_CASE("Peer Functional Testing...", "[PeersManager class]")
{
    Peer myself;
    auto metafile = bitusk::MetafileObject::readMetaStrFromFile("./test-bt.torrent");
    std::cout << std::endl << "info hash.." << reinterpret_cast<const char*>(bitusk::MetafileObject::CaculateSha1(metafile->getString("info")).c_str())<< std::endl;
    myself.info_hash = bitusk::MetafileObject::CaculateSha1(metafile->getString("info"));
    REQUIRE(myself.info_hash.size() == 20 );
    //myself.info_hash = StringToUstring(src);
    myself.peer_id = bitusk::StringToUstring(metafile->getString("peer id"));
    REQUIRE( myself.info_hash.size() == 20 );

    //std::cout <<"peer id .." << metafile->getString("peer id") << std::endl;
    //std::cout <<"peer id .." << metafile->getString("name") << std::endl;
    std::cout << "Info hash " << reinterpret_cast<const char*>(myself.info_hash.c_str()) << std::endl;
    std::cout << "peer id " << reinterpret_cast<const char*>(myself.peer_id.c_str()) << std::endl;
    PeersManager* pm = PeersManager::Instance();
    REQUIRE( pm == nullptr);
    SECTION("This peer context...") {
        
    }
}



TEST_CASE("PeersManager::Instance() testing", "[class PeersManager]") {
    
    PeersManager* pm = PeersManager::Instance();
    REQUIRE( pm == nullptr);
}
