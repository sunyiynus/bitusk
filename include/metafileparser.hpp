// Copyright 2021 Uttep.
// License(BSD)
// Author: Uttep
// Access the key of Bittorrent Protocol 
#ifndef BITUSK_SRC_METAFILEPARSER_H
#define BITUSK_SRC_METAFILEPARSER_H

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace bitusk{




class ParserMetafileInterface;
class AnnounceList;
class PieceLength;
class FileName;
class FileLength;
class Files;
class Info;
class PeerId;


struct FileNode{
  FileNode() = default;
  FileNode(const FileNode& f) = default;
  FileNode& operator=(const FileNode&) = default;
  ~FileNode() = default;
  std::string path;
  size_t length;
};

class MetafileObject{

public:
  friend class AnnounceList;
  friend class Files;
  friend class PieceLength;
  friend class Pieces;
  friend class FileName;
  friend class FileLength;
  friend class Info;
  friend class PeerId;

  using dict_type=std::map<std::string,std::string>;
  
  MetafileObject();
  MetafileObject( const MetafileObject&) = delete;
  MetafileObject(const std::string& ms);
  ~MetafileObject() = default;
  
  // Reads metafile from FILE and generates a object contains metafile.
  // Usage:
  //   auto ob = MetafileObject::readMetaStrFromFile("your/BittorentFile/path");
  //   int len = ob->getInt("filelength");
  // Returned a shared_ptr<MetafileObject>
  static std::shared_ptr<MetafileObject> readMetaStrFromFile(const std::string& file);
  static std::basic_string<char> CaculateSha1ToString(const std::string& str);
  static const std::basic_string<unsigned char> CaculateSha1(const std::string& str);
  bool IsMetaStrOk() const;
  const std::string& MetaStr() const;

  // Some key's value is int, so using this func will be easy.
  const int                       getInt(const std::string& key) ;
  const std::string               getString(const std::string& ) ;
  const std::vector<std::string>  getList(const std::string& ) ;
  const dict_type	                getDict(const std::string& ) ;

  const std::vector<FileNode> getFiles();

  // For testing convension.
  const int GetTmpInt() const;
  const std::string& GetTmpStr() const;
  const std::vector<std::string>& GetTmpList() const;
  const dict_type& GetTmpDict() const;

protected:
  void init();
  void Controler(const std::string& key);

private:
  std::string metaStr_;
  std::map<std::string, std::shared_ptr<ParserMetafileInterface>> func_;
  // For return value
  int tmpint;
  std::string tmpstr;
  std::vector<std::string> tmplist;
  dict_type tmpdict;

  std::vector<FileNode> files_;
};


class ParserMetafileInterface{
public:
  virtual bool operator()(MetafileObject* obj) = 0;
};


class AnnounceList: public ParserMetafileInterface {
public:
  virtual bool operator()(MetafileObject* obj) override;
};


class Files: public ParserMetafileInterface {
public:
  virtual bool operator()(MetafileObject* obj) override;
  void FindSingleFile(std::string& metastr);
  void FindMutilFiles(std::string& metastr);
  const std::vector<FileNode>& GetFiles() const;
private:
  std::vector<FileNode> files_;
};


class PieceLength: public ParserMetafileInterface {
public:
  virtual bool operator()(MetafileObject* obj) override;
};


class Pieces: public ParserMetafileInterface {
public:
  virtual bool operator()(MetafileObject* obj) override;
};


class FileName: public ParserMetafileInterface {
public:
  virtual bool operator()(MetafileObject* obj) override;
};


class FileLength: public ParserMetafileInterface {
public:
  virtual bool operator()(MetafileObject* obj) override;
};


class Info: public ParserMetafileInterface {
public:
  virtual bool operator()(MetafileObject* obj) override;
  bool findPair(std::string::iterator& itr, std::string& str);
};


class PeerId: public ParserMetafileInterface {
public:
  virtual bool operator()(MetafileObject* obj) override;
};


inline std::basic_string<unsigned char> StringToUstring(const std::string& src)
{
    std::basic_string<unsigned char> target( src.cbegin(), src.cend());
    return target;
}



} // namespace bitusk

#endif // BITUSK_SRC_METAFILEPARSER_H
