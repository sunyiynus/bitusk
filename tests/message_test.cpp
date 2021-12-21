#include <iostream>
#include <cstdio>
#include <sstream>
#include "catch.hpp"
#include "message.hpp"
#include "peer.hpp"
#include "metafileparser.hpp"

using namespace std;

TEST_CASE("Test MsgTyper Function ItoC CtoI", "[Test Senciao]")
{
    REQUIRE(1==1);

    MsgTyper::uostringstream os;
    REQUIRE( MsgTyper::IntToChar(10, os) == 0);
    MsgTyper::ustring ustr (os.str());
    REQUIRE( MsgTyper::CharToInt(ustr.c_str()) == 10);
}

using ustring = MsgTyper::ustring;

TEST_CASE("Test MessageGen ", "[Test Message Generating]") {
        Peer myself;
    auto metafile = bitusk::MetafileObject::readMetaStrFromFile("./test-bt.torrent");
    myself.info_hash = bitusk::MetafileObject::CaculateSha1(metafile->getString("info"));
    //myself.info_hash = bitusk::StringToUstring(src);
    myself.peer_id = bitusk::StringToUstring(metafile->getString("peer id"));
    REQUIRE( myself.info_hash.size() ==20);
    REQUIRE( myself.peer_id.size() ==20);

    SECTION("Test HandShakeMsg...") {
        HandShakeMsg gen;
        ustring msg = gen.GenerateMessage(myself);
        cout <<endl <<reinterpret_cast<const char*>(msg.c_str()) << std::endl;
        REQUIRE( msg.size() == 68);
    }

    SECTION("Test KeepAliveMsg...") {
        KeepAliveMsg gen;
        ustring msg = gen.GenerateMessage(myself);
        REQUIRE( msg.size() == 4);
    }

    SECTION("Test ChokedMsg...") {
        ChokedMsg gen;
        ustring msg = gen.GenerateMessage(myself);
        REQUIRE( msg.size() == 5);
    }

    SECTION("Test UnChockdMsg...") {
        UnChockdMsg gen;
        ustring msg = gen.GenerateMessage(myself);
        REQUIRE( msg.size() == 5);
    }

    SECTION("Test InterestedMsg...") {
        InterestedMsg gen;
        ustring msg = gen.GenerateMessage(myself);
        REQUIRE( msg.size() == 5);
    }

    SECTION("Test UnInterestedMsg...") {
        UnInterestedMsg gen;
        ustring msg = gen.GenerateMessage(myself);
        REQUIRE( msg.size() == 5);
    }

    SECTION("Test HaveMsg...") {
        HaveMsg gen;
        std::vector<int> piece {4};
        std::string bits("1 0 1 1 1");
        std::istringstream is(bits.c_str());
        myself.bitmap.Read(is);
        REQUIRE( myself.bitmap.Size() == 5);
        gen.Set(piece);
        ustring msg = gen.GenerateMessage(myself);
        REQUIRE( msg.size() == 9);
    }

    SECTION("Test HaveMsg...") {
        HaveMsg gen;
        std::vector<int> piece {4};
        gen.Set(piece);
        ustring msg = gen.GenerateMessage(myself);
        REQUIRE( msg.size() == 9);
    }

    SECTION("Test BitfieldMsg...") {
        BitfieldMsg gen;
        //std::vector<int> piece {4};
        std::string bits("1 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0");
        std::istringstream is(bits.c_str());
        myself.bitmap.Read(is);
        REQUIRE( myself.bitmap.Size() == 18);
        ustring msg = gen.GenerateMessage(myself);
        REQUIRE( msg.size() == ((myself.bitmap.Size()%8 == 0)? (myself.bitmap.Size() /8): (myself.bitmap.Size()/8 +1) + 4 + 1));
        //cout << "Test BitfieldMsg... "<<reinterpret_cast<const char*>(msg.c_str()) << endl;
    }


    SECTION("Test RequestMsg...") {
        RequestMsg gen;

        myself.request_queue.emplace(1,2,3);
        REQUIRE( myself.request_queue.size() == 1);
        //std::vector<int> piece {4};
        ustring msg = gen.GenerateMessage(myself);
        REQUIRE( msg.size() == 17);
        REQUIRE( myself.request_queue.size() == 0);
        //cout << "Test BitfieldMsg... "<<reinterpret_cast<const char*>(msg.c_str()) << endl;
    }


    SECTION("Test RequestMsg...") {
        RequestMsg gen;
        REQUIRE( myself.request_queue.empty() == true);
        //std::vector<int> piece {4};
        ustring msg = gen.GenerateMessage(myself);
        REQUIRE( msg.size() == 0);
        REQUIRE( myself.request_queue.size() == 0);
        //cout << "Test BitfieldMsg... "<<reinterpret_cast<const char*>(msg.c_str()) << endl;
    }
}


TEST_CASE("Test GeneratorFactory ", "[Test generator factory]") {
    Peer myself;
    auto metafile = bitusk::MetafileObject::readMetaStrFromFile("./test-bt.torrent");
    myself.info_hash = bitusk::MetafileObject::CaculateSha1(metafile->getString("info"));
    //myself.info_hash = bitusk::StringToUstring(src);
    myself.peer_id = bitusk::StringToUstring(metafile->getString("peer id"));
    REQUIRE( myself.info_hash.size() ==20);
    REQUIRE( myself.peer_id.size() ==20);

    std::shared_ptr<AbsMsgFactory> msggenfactory  = std::make_shared<MsgGeneratorFactory>();
    
    SECTION("Test HandShakeMsg...") {
        std::shared_ptr<AbsMessageGenerator> gen (
            msggenfactory->GetMsgGenerator(myself, MsgNum::kHandShake));
        ustring msg = gen->GenerateMessage(myself);
        cout <<endl <<reinterpret_cast<const char*>(msg.c_str()) << std::endl;
        REQUIRE( msg.size() == 68);
    }

        SECTION("Test KeepAliveMsg...") {
        std::shared_ptr<AbsMessageGenerator> gen (
            msggenfactory->GetMsgGenerator(myself, MsgNum::kKeepAlive));
        ustring msg = gen->GenerateMessage(myself);
        REQUIRE( msg.size() == 4);
    }

    SECTION("Test ChokedMsg...") {
        std::shared_ptr<AbsMessageGenerator> gen (
            msggenfactory->GetMsgGenerator(myself, MsgNum::kChoke));
        ustring msg = gen->GenerateMessage(myself);
        REQUIRE( msg.size() == 5);
    }

    SECTION("Test UnChockdMsg...") {
        std::shared_ptr<AbsMessageGenerator> gen (
            msggenfactory->GetMsgGenerator(myself, MsgNum::kUnchoke));
        ustring msg = gen->GenerateMessage(myself);
        REQUIRE( msg.size() == 5);
    }

    SECTION("Test InterestedMsg...") {
        std::shared_ptr<AbsMessageGenerator> gen (
            msggenfactory->GetMsgGenerator(myself, MsgNum::kInterested));
        ustring msg = gen->GenerateMessage(myself);
        REQUIRE( msg.size() == 5);
    }

    SECTION("Test UnInterestedMsg...") {
        std::shared_ptr<AbsMessageGenerator> gen (
            msggenfactory->GetMsgGenerator(myself, MsgNum::kUnInterested));
        ustring msg = gen->GenerateMessage(myself);
        REQUIRE( msg.size() == 5);
    }

    SECTION("Test HaveMsg...") {
        std::shared_ptr<AbsMessageGenerator> gen (
            msggenfactory->GetMsgGenerator(myself, MsgNum::kHave));
        std::vector<int> piece {4};
        std::string bits("1 0 1 1 1");
        std::istringstream is(bits.c_str());
        myself.bitmap.Read(is);
        REQUIRE( myself.bitmap.Size() == 5);
        gen->Set(piece);
        ustring msg = gen->GenerateMessage(myself);
        REQUIRE( msg.size() == 9);
    }

    SECTION("Test HaveMsg...") {
        std::shared_ptr<AbsMessageGenerator> gen (
            msggenfactory->GetMsgGenerator(myself, MsgNum::kHave));
        std::vector<int> piece {4};
        gen->Set(piece);
        ustring msg = gen->GenerateMessage(myself);
        REQUIRE( msg.size() == 9);
    }

    SECTION("Test BitfieldMsg...") {
        std::shared_ptr<AbsMessageGenerator> gen (
            msggenfactory->GetMsgGenerator(myself, MsgNum::kBitfield));
        std::string bits("1 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0");
        std::istringstream is(bits.c_str());
        myself.bitmap.Read(is);
        REQUIRE( myself.bitmap.Size() == 18);
        ustring msg = gen->GenerateMessage(myself);
        REQUIRE( msg.size() == ((myself.bitmap.Size()%8 == 0)? (myself.bitmap.Size() /8): (myself.bitmap.Size()/8 +1) + 4 + 1));
        //cout << "Test BitfieldMsg... "<<reinterpret_cast<const char*>(msg.c_str()) << endl;
    }


    SECTION("Test RequestMsg...") {
        std::shared_ptr<AbsMessageGenerator> gen (
            msggenfactory->GetMsgGenerator(myself, MsgNum::kRequest));
        myself.request_queue.emplace(1,2,3);
        REQUIRE( myself.request_queue.size() == 1);
        //std::vector<int> piece {4};
        ustring msg = gen->GenerateMessage(myself);
        REQUIRE( msg.size() == 17);
        REQUIRE( myself.request_queue.size() == 0);
        //cout << "Test BitfieldMsg... "<<reinterpret_cast<const char*>(msg.c_str()) << endl;
    }


    SECTION("Test RequestMsg...") {
        std::shared_ptr<AbsMessageGenerator> gen (
            msggenfactory->GetMsgGenerator(myself, MsgNum::kRequest));
        REQUIRE( myself.request_queue.empty() == true);
        //std::vector<int> piece {4};
        ustring msg = gen->GenerateMessage(myself);
        REQUIRE( msg.size() == 0);
        REQUIRE( myself.request_queue.size() == 0);
        //cout << "Test BitfieldMsg... "<<reinterpret_cast<const char*>(msg.c_str()) << endl;
    }

}


