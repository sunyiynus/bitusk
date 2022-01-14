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
#include "log.hpp"
#include "basic.hpp"


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
    //init log
    Logger& logger = Logger::Instance();
    logger.Init(&std::cout);
    LOGGER(loggers);

    // read bittorrent file to get nessary info
    metafile = bitusk::MetafileObject::readMetaStrFromFile(metafilename);
    std::cout << "read metafile from file : " << metafilename << std::endl;

    // get singleton PeersManager
    
    PeersManager* pm = PeersManager::InitInstance(ioserver);
    assert(pm != nullptr);
    Peer& myself = pm->GetMyself();
    std::cout << "get singleton PeersManager and setting some variable .. " << std::endl;

    // initialize myself peer struct with
    // info_hash peer_id file_total_size
    //std::cout << metafile->getString("info") << std::endl;
    myself.info_hash = bitusk::MetafileObject::CaculateSha1(metafile->getString("info"));
    std::cout << "info Hash ok !" << std::endl;
    myself.peer_id = bitusk::StringToUstring(metafile->getString("peer id"));

    std::cout << "Setting Peers info hash and peer id of myself " << std::endl;

    auto files = metafile->getFiles();
    size_t total  = 0;
    for( auto& f: files) {
        std::cout << "file size " << f.length << std::endl;
        total += f.length; 
    }
    myself.scounter.file_total_size = total;
    loggers.Debug() << "fiel total size : " << total << "\n";
    std::cout << "setting file total size of bittorrent..." <<total <<    std::endl;



    // initial file manager

    // TODO
    // initial datacache



    // initial peer manager


    
    return true;
}




int main(int argc, char* argv[]) {
    std::vector<std::string> args = WrapArgv(argc, argv);
    if(args.empty()) exit(1);

    if( !InitializeEnv(args[0])) {
        std::cout << "Something wrong, but Im not define the error now.." << std::endl;
    }
    LOGGER(logger);
    PeersManager* pmanager = PeersManager::Instance();
    assert( pmanager != nullptr );

#ifdef DEBUG_MACRO
    logger.Debug() << "Begin to debug peer exchange....";
    assert( args.size() > 1);
    if( args[1] == "-s") {
        logger.Debug() << "Im will be a server";
        pmanager->SetAcceptor();
        pmanager->StartRecvPeerConnection();
    } else if( args[1] == "-c") {
        logger.Debug() << "Im will be a client";
        ip::tcp::endpoint ep( ip::address::from_string("127.0.0.1"), 6969);
        pmanager->AddPeer(ep);
    }

#else
    auto announce = metafile->getList("announce-list");

    TrackersManager::ptr tmanager = TrackersManager::Start(announce, ioserver);
    pmanager->StartRecvPeerConnection();
    logger.Debug() << "Starting recv peer connection..";
#endif

    ioserver.run();
}


