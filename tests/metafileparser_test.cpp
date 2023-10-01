#include "metafileparser.hpp"
#include "catch2/catch_test_macros.hpp"

#include <iostream>
#include <string>


TEST_CASE("files get class test ", ) {
    std::string str = "5:filesld6:lengthi835626448e4:pathl57:The.Green.Knight.2021.720p.WEBRip.800MB.x264-GalaxyRG.mkveed6:lengthi718e4:pathl42:[TGx]Downloaded from torrentgalaxy.to .txteee4:name58:The.Green.Knight.2021.720p.WEBRip.800MB.x264-GalaxyRG[TGx]12:piece lengthi524288e6:pieces3";
    bitusk::Files fp;
    fp.FindMutilFiles(str);
    auto files = fp.GetFiles();
    REQUIRE( files.size() == 2);
    
}

TEST_CASE("files get", "[class Files]") {
    using namespace bitusk;

    auto metafile = bitusk::MetafileObject::readMetaStrFromFile("./test-bt.torrent");
    auto files = metafile->getFiles();
    REQUIRE( files.size() == 2);
    //REQUIRE( files[0].path.size() == (58 + 57 +1) );
    //REQUIRE( files[1].path.size() == (58 + 42 +1) );

    auto announce = metafile->getList("announce-list");
    REQUIRE( announce.size() > 0 );
    std::cout << "Files : " << std::endl;
    std::cout << files[0].path << std::endl;
    std::cout << files[0].length << std::endl;
    std::cout << files[1].path << std::endl;
    std::cout << files[1].length << std::endl;
}