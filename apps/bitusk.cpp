#include <cstdlib>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>
#include <string>
#include <deque>


#include <boost/asio.hpp>

#include "bitfield.hpp"
#include "metafileparser.hpp"
#include "peer.hpp"
#include "tracker.hpp"
#include "datacache.hpp"


boost::asio::io_context ioserver;
std::shared_ptr<bitusk::MetafileObject> metafile;

std::vector<std::string> WrapArgv(int argc, char* argv[]) {
    std::deque<std::string> args;
    for( int i = argc; i > 1; i--) {
        args.push_front(std::string(argv[i-1]));
    }
    return std::vector<std::string>(args.begin(), args.end());
}


//bool InitializePeersManager(bitusk::MetafileObject& metafile)



bool InitializeEnv(const std::string& metafilename) {

    // read bittorrent file to get nessary info
    metafile = bitusk::MetafileObject::readMetaStrFromFile(metafilename);
    std::cout << "read metafile from file : " << metafilename << std::endl;

    // get singleton PeersManager
    PeersManager* pm = PeersManager::GetInstance();
    Peer& myself = pm->GetMyself();
    std::cout << "get singleton PeersManager and setting some variable .. " << std::endl;

    // initialize myself peer struct with
    // info_hash peer_id file_total_size
    myself.info_hash = bitusk::MetafileObject::CaculateSha1(metafile->getString("info"));
    myself.peer_id = bitusk::StringToUstring(metafile->getString("peer id"));

    std::cout << "Setting Peers info hash and peer id of myself " << std::endl;

    auto files = metafile->getFiles();
    size_t total  = 0;
    for( auto& f: files) {
        std::cout << "file size " << f.length << std::endl;
        total += f.length; 
    }
    myself.scounter.file_total_size = total;
    std::cout << "setting file total size of bittorrent..." <<total <<    std::endl;
    return true;
}




int main(int argc, char* argv[]) {
    std::vector<std::string> args = WrapArgv(argc, argv);
    if(args.empty()) exit(1);

    if( !InitializeEnv(args[0])) {
        std::cout << "Something wrong, but Im not define the error now.." << std::endl;
    }

    auto announce = metafile->getList("announce-list");

    TrackersManager::ptr tmanager = TrackersManager::Start(announce, ioserver);


    ioserver.run();
}


