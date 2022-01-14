#include <cstddef>
#include <iterator>
#include <stack>
#include <fstream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <iomanip>
#include <ctime>

#include <boost/uuid/detail/sha1.hpp>

#define DEBUG

#ifdef DEBUG
#include <algorithm>
#include "debug.hpp"
#endif

#include "metafileparser.hpp"
#include "bcode.hpp"

using namespace bitusk;
using namespace boost::uuids::detail;

std::string MetafileObject::CaculateSha1ToString(const std::string& str) {
    sha1 sha;
    sha.process_bytes(str.c_str(), str.size());
    unsigned int digest[5];
    sha.get_digest(digest);

    std::basic_ostringstream<char> buf;
    for( int i = 0; i < 5; ++i) {
        buf << std::hex << std::setfill('0') << std::setw(8) << digest[i];
    }
    //const unsigned char* uc = reinterpret_cast<const unsigned char*>(buf.str());
    
    return buf.str();
}

const std::basic_string<unsigned char> MetafileObject::CaculateSha1(const std::string& str)
{
    std::basic_ostringstream<unsigned char> buf;
    if( str.empty()) return std::basic_string<unsigned char>(buf.str());
    sha1 sha;
    unsigned int digest[5];
    sha.process_bytes(str.c_str(), str.size());
    sha.get_digest(digest);

    unsigned char* itr = reinterpret_cast<unsigned char*>(&digest[0]);

    for( int i = 0; i < 20; ++i)
    {
      buf << itr[i];
      //std::cout << " << "<< i << ">> " << itr[i] << std::endl;
//    std::cout << " size of " << std::basic_string<unsigned char>(buf.str()).size();
    }

    //std::cout << " out of loop size of " << std::basic_string<unsigned char>(buf.str()).size();

    return std::basic_string<unsigned char>(buf.str());
}



MetafileObject::MetafileObject() {
  init();
}


MetafileObject::MetafileObject(const std::string& ms) : metaStr_(ms) {
  init();
}


bool MetafileObject::IsMetaStrOk() const {
  return !this->metaStr_.empty();
}


const std::string& MetafileObject::MetaStr() const {
  return this->metaStr_;
}


std::shared_ptr<MetafileObject> MetafileObject::readMetaStrFromFile( const std::string& file) {
  std::ifstream f {file};
  if( f ){
    std::string tmpStr {std::istreambuf_iterator<char>(f),
						std::istreambuf_iterator<char>()};
    auto obj =  std::make_shared<MetafileObject>(tmpStr);
    return obj;
  }
  return std::make_shared<MetafileObject>();
}


const int MetafileObject::getInt(const std::string& key) {
  Controler(key);
  return tmpint;
}


const std::string MetafileObject::getString(const std::string& key) {
  Controler(key);
  return tmpstr;
}


const std::vector<std::string> MetafileObject::getList(const std::string& key) {
  Controler(key);
  return tmplist;
}


const MetafileObject::dict_type MetafileObject::getDict(const std::string & key) {
  Controler(key);
  return  tmpdict;
}


const std::vector<FileNode> MetafileObject::getFiles() {
  if( files_.empty() ) {
    Controler("files");
  }
  return files_;
}


const int MetafileObject::GetTmpInt() const {
  return tmpint;
}


const std::string& MetafileObject::GetTmpStr() const {
  return tmpstr;
}


const std::vector<std::string>& MetafileObject::GetTmpList() const {
  return tmplist;
}


const MetafileObject::dict_type& MetafileObject::GetTmpDict() const {
  return tmpdict;
}


void MetafileObject::init() {
  tmpint = 0;
  func_["announce-list"] = std::make_shared<AnnounceList>();
  func_["announce"] = std::make_shared<AnnounceList>();
  func_["files"] = std::make_shared<Files>();
  func_["piece length"] = std::make_shared<PieceLength>();
  func_["pieces"] = std::make_shared<Pieces>();
  func_["name"] = std::make_shared<FileName>();
  func_["info"] = std::make_shared<Info>();
  func_["peer id"] = std::make_shared<PeerId>();
}


void MetafileObject::Controler(const std::string& key) {
  if ( func_.find(key) == func_.end() ) return;
  (*func_[key])(this);
}


bool AnnounceList::operator()(MetafileObject* obj) {
  obj->tmplist.clear();
  if( obj->metaStr_.empty()) return false;

  std::string::size_type pos = 0;
  std::vector<std::string> announces;
  if( (pos = obj->metaStr_.find("13:announce-list")) != std::string::npos) {
    std::string::iterator itr = obj->metaStr_.begin() + pos + 13 + 3;
    std::stack<char> stk;
    stk.push(*itr++);
    while ( !stk.empty() ) {
      switch (*itr) {
        case 'l':
            stk.push(*itr++);
            break;
        case 'e':
            stk.pop();
            itr++;
            break;
        default:
            announces.push_back(Bcode::parseStr(itr));
            break;
      }
    }
    obj->tmplist = announces;
#ifdef DEBUG
    PRINT_CONTAINER(announces);
#endif
    return true;
  } else {
    pos = obj->metaStr_.find("8:announce");
    std::string::iterator itr = obj->metaStr_.begin() + pos + 8 + 2;
    announces.push_back(Bcode::parseStr(itr));
    obj->tmplist = announces;
#ifdef DEBUG
    PRINT_CONTAINER(announces);
#endif
    return true;
  }
  return false;
}


// Finished
// Im not sure.
bool Files::operator()(MetafileObject *obj) {
  obj->tmpint = 0;
  if( obj->metaStr_.empty()) return false;

  if( obj->metaStr_.find("files") == std::string::npos ) {
    FindSingleFile(obj->metaStr_);
  } else {
    FindMutilFiles(obj->metaStr_);
  }

  obj->files_ = GetFiles();
  return true;
}

const std::vector<FileNode>& Files::GetFiles() const {
  return files_;
}


void Files::FindSingleFile(std::string& metastr) {
  std::string::size_type pos;
  std::string::iterator itr = metastr.begin();
  FileNode node;
  if( ( pos = metastr.find("6:length")) != std::string::npos) {
    itr += (pos + 6 + 2);
    node.length = Bcode::parseInt( ++itr );
  }
  if( ( pos = metastr.find("4:name")) != std::string::npos) {
    itr += (pos + 4 + 2);
    node.path = Bcode::parseStr( itr );
  }
  files_.push_back(node);
}


void Files::FindMutilFiles(std::string& metastr) {
  std::string::size_type pos;
  //std::string::iterator itr;
  if( ( pos = metastr.find("5:files")) == std::string::npos) {
    throw std::logic_error("can't find files segement..");
    return;
  }

  std::string::size_type namepos = metastr.find("4:name");
  std::string name;
  if( namepos != std::string::npos) {
    std::string::iterator nameitr = (metastr.begin() + namepos + 4 + 2);
    name = Bcode::parseStr( nameitr );
  }


  long target = std::string::npos;
  while( (target = metastr.find("6:length", pos)) != std::string::npos) {
    FileNode node;
    std::string::iterator tmpitr = metastr.begin() + target + 6 + 2;
    node.length = Bcode::parseInt(++tmpitr);
    
    if( ( target = metastr.find("4:path", pos)) != std::string::npos) {
      //node.path =  "/" + name ;
      tmpitr = metastr.begin() + target + 4 + 2;

      node.path = name + "/" + Bcode::parseStr( ++tmpitr );
      files_.push_back(node);
      pos = target + 4 + 3;
    }
  }
}


bool PieceLength::operator()(MetafileObject *obj) {
  obj->tmpint = 0;
  if( obj->metaStr_.empty()) return false;
  
  std::string::size_type pos = 0;
  const std::string& reftometa = obj->metaStr_;
  if( (pos = reftometa.find("12:piece length")) == std::string::npos) {
    return false;
  }

  std::string::iterator itr = obj->metaStr_.begin() + pos + 12 + 3;
  obj->tmpint = Bcode::parseInt(++itr);
  return true;
}


bool Pieces::operator()(MetafileObject *obj) {
  obj->tmpstr.clear();
  if( obj->metaStr_.empty()) return false;

  std::string::size_type pos = 0;
  const std::string& reftometa = obj->metaStr_;
  if( (pos = reftometa.find("6:pieces")) == std::string::npos) {
    return false;
  }

  std::string::iterator itr = obj->metaStr_.begin() + pos + 6 + 2;
  obj->tmpstr = Bcode::parseStr(itr);
  return true;
}


bool FileName::operator()(MetafileObject *obj) {
  obj->tmpstr.clear();
  if( obj->metaStr_.empty()) return false;

  std::string::size_type pos = 0;
  const std::string& reftometa = obj->metaStr_;
  if( (pos = reftometa.find("4:name")) == std::string::npos) {
    return false;
  }

  std::string::iterator itr = obj->metaStr_.begin() + pos + 4 + 2;
  obj->tmpstr = Bcode::parseStr(itr);
  return true;
}

// abandanded
bool FileLength::operator()(MetafileObject *obj) {
  obj->tmpint = 0;
  if( obj->metaStr_.empty()) return false;

  std::string::size_type pos = 0;
  const std::string& reftometa = obj->metaStr_;
  if( (pos = reftometa.find("6:length")) == std::string::npos) {
    return false;
  }

  std::string::iterator itr = obj->metaStr_.begin() + pos + 6 + 2;
  obj->tmpint = Bcode::parseInt(++itr);
  return true;
}


bool Info::operator()(MetafileObject *obj) {
  obj->tmpstr.clear();
  if( obj->metaStr_.empty()) return false;

  std::string::size_type pos = 0;
  const std::string& reftometa = obj->metaStr_;
  if( (pos = reftometa.find("4:info")) == std::string::npos) {
    return false;
  }

  std::string::iterator itr = obj->metaStr_.begin() + pos + 4 + 2;
  std::string::iterator startitr = itr;

  Bcode::findPair(itr);
  //findPair(itr, obj->metaStr_);
  obj->tmpstr = std::string(startitr, itr);
  return true;
}


bool Info::findPair(std::string::iterator& itr, std::string& str)
{
  int push_pop = 0;
  std::string::iterator end;
  
  for(; itr != str.end();)
  {
    if( *itr  == 'd') {
      push_pop++;
      itr++;
    } else if( *itr == 'l') {
      push_pop++;
      itr++;
    } else if( *itr == 'i' ) {
      itr++;
      if( itr == str.end() ) {
        return false;
      }

      while( *itr !=  'e') {
        if(  (itr + 1)  == str.end()) return false;
        else itr++;
      }

      itr++;
    } else if( (*itr >= '0') && (*itr <= '9') ) {
      int number = 0;
      while( (*itr >= '0') && (*itr <='9') ) {
        number = number * 10 + *itr -'0';
        itr++;
      }
      itr++;
      itr += number;
    } else if ( *itr == 'e' ) {
      push_pop--;
      if( push_pop == 0 ) { end = itr; break; }
      else itr++;
    } else {
      return false;
    }

    if( itr == str.cend() ) return false;
  }

  return false;
}


bool PeerId::operator()(MetafileObject *obj) {
  obj->tmpstr.clear();
  if( obj->metaStr_.empty()) return false;
  
  std::random_device r;
  static std::default_random_engine dre(r());
  static std::uniform_int_distribution<long> di (111111111111, 999999999999);

  std::ostringstream ss;
  ss << "-bitusk-" << di(dre);

  obj->tmpstr = std::string(ss.str());
  return true;
}



















