#ifndef BITUSK_SRC_DATACACHE_H__
#define BITUSK_SRC_DATACACHE_H__

#include <bits/types/FILE.h>
#include <algorithm>
#include <boost/circular_buffer.hpp>

#include <sys/stat.h>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <istream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include "metafileparser.hpp"
#include "peer.hpp"

//  :
//  |------
//  |------
//  |------
//  :

#define WRITE_TO_SINGLE_FILE
#define ASSERT_NOT_NULPTR(x) assert(x != nullptr)

typedef std::basic_string<unsigned char> ustring;

struct Node {
  Node()
      : index(0),
        offset(0),
        in_use(false),
        write_read(true),
        is_writed(true),
        access_count(0) {}
  Node(int i, int o)
      : index(i),
        offset(o),
        in_use(true),
        write_read(true),
        is_writed(true),
        access_count(0) {}

  Node(const Node& n) = default;
  Node(Node&& n) = default;
  Node& operator=(const Node& n) = default;
  ustring buffer;
  int index;
  int offset;

  bool in_use;
  bool write_read;
  bool is_writed;

  bool isFull() const { return (buffer.size() == 16 * 1024) ? true : false; }

  int access_count;
  Node& operator++() {
    access_count++;
    return *this;
  }

  Node operator++(int i) {
    this->operator++();
    return *this;
  }

  Node& Write(std::ostream& os) {
    /*
        os.seekp(index * 256 * 1024 + offset * 16 * 1024);
        os << reinterpret_cast<const char*>(buffer.c_str());
        is_writed = true;
        */
    return *this;
  }

  Node& Read(std::istream& is) {
    //is.seekg(index * 256 * 1024 + offset);

    return *this;
  }

  operator bool() { return in_use; }
};

template <typename T>
int sum_over_vector(const std::vector<T>& v,
                    const std::function<int(const T&)>& member) {
  return std::accumulate(v.begin(), v.end(), 0, [&](int sum, const T& curr) {
    return sum + member(curr);
  });
}

struct File {
  std::string path;
  size_t length;
  std::shared_ptr<std::fstream> fs;
};

class AbsFileManager {
 private:
 protected:
  AbsFileManager() = default;
  AbsFileManager(const AbsFileManager&);
  AbsFileManager(const std::string& dd) : download_dir_(dd) {}
  AbsFileManager(const std::string& dd, const std::vector<File>& files)
      : download_dir_(dd), files_info_(files) {}

 public:
  // read from disk
  virtual bool LoadingFiles() = 0;
  virtual void ReadSliceNodeFromDisk(Node& node) = 0;
  virtual const Node ReadSliceNodeFromDisk(const size_t index,
                                           const size_t offset,
                                           const size_t length) = 0;
  virtual bool WriteSliceNodeToDisk(const Node& node) = 0;
  virtual bool IntegrateFiles() = 0;

 public:
 protected:
  std::string download_dir_;
  std::vector<File> files_info_;
};

class FilesManagerSV : public AbsFileManager {
 private:
  std::shared_ptr<std::iostream> fs_;
  size_t piece_length_;
  size_t total_length_;

 private:
  FilesManagerSV() = default;
  FilesManagerSV(const FilesManagerSV&) = default;
  FilesManagerSV(const std::string& dd, const std::vector<File>& file);

 public:
  // must firstly loading files
  virtual bool LoadingFiles() override;
  virtual void ReadSliceNodeFromDisk(Node& node) override;
  virtual const Node ReadSliceNodeFromDisk(size_t index, size_t offset,
                                           size_t length) override;
  virtual bool WriteSliceNodeToDisk(const Node& node) override;
  virtual bool IntegrateFiles() override;

 public:
  static FilesManagerSV* GetInstance(const std::string& dd,
                                     const std::vector<File>& file);
  static FilesManagerSV* GetInstance();

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
  inline static std::atomic<FilesManagerSV*> mfm_instance;
  inline static std::mutex m_mutex;
#else
  static std::atomic<FilesManagerSV*> mfm_instance;
  static std::mutex m_mutex;
#endif

 public:
  const bool IsFileReady() const;
};

inline AbsFileManager& operator<<(AbsFileManager& os, Node& node) {
  os.WriteSliceNodeToDisk(node);
  return os;
}

inline AbsFileManager& operator>>(AbsFileManager& is, Node& node) {
  is.ReadSliceNodeFromDisk(node);
  return is;
}

struct PieceNode {
  std::vector<Node> slices;

 public:
  PieceNode() : slices(16, Node()) {}
  const bool IsCompletePiece() {
    slices.erase(
        std::unique(slices.begin(), slices.end(),
                    [](auto& a, auto& b) { return a.offset == b.offset; }),
        slices.end());
    std::sort(slices.begin(), slices.end(),
              [](auto& a, auto& b) { return a.offset < b.offset; });
    return true;
  }

  void Add(Node& node) { slices.push_back(node); }

  const std::vector<Node>& Get() const { return slices; }
};

inline PieceNode& operator+=(PieceNode& pnode, Node& node) {
  pnode.Add(node);
  return pnode;
}

// enhance FilesManager
class FilesManagerDecorator : public AbsFileManager {
 public:
  static FilesManagerDecorator& GetInstance();

 private:
  FilesManagerDecorator() : fmanager_(nullptr) {}
  FilesManagerDecorator(AbsFileManager* fm) : fmanager_(fm) {}

 public:
  // inheret api
  virtual bool LoadingFiles() override;
  virtual void ReadSliceNodeFromDisk(Node& node) override;
  virtual const Node ReadSliceNodeFromDisk(const size_t index,
                                           const size_t offset,
                                           const size_t length) override;
  virtual bool WriteSliceNodeToDisk(const Node& node) override;
  virtual bool IntegrateFiles() override;

 public:
  const PieceNode ReadPieceFromDisk(const size_t index, const size_t piece_n);
  bool WritePieceToDisk(const PieceNode& piece);
  const size_t GetTotalSize() const;

 private:
  AbsFileManager* fmanager_;
};

class CacheV1 {
 private:
  typedef std::vector<Node> PieceNodeType;

  std::vector<PieceNodeType> cache_;
  std::map<size_t, std::vector<PieceNodeType>::iterator> hash_table_;
  Peer* myself_;

 public:
  // some other operation

  void SetMyselfPeer(Peer* myself) {
    if (!myself)
      return;
    myself_ = myself;
  }

 public:
  CacheV1() : cache_(64, std::vector<Node>()) {}
  CacheV1(size_t ps, size_t ss)
      : cache_(ps, std::vector<Node>()), myself_(nullptr) {}

  std::vector<PieceNodeType>::iterator SwapOut() {
    // 1. 找到访问次数最少的Piece node
    // 2. 更改此piece node中所有 slice node的in use flag
    // 3. 在hash table中删除该piece node的记录
    // 4. 返回该piece node 的itr
    auto acc = [&](int sum, const Node& node) {
      return sum + node.access_count;
    };

    std::vector<PieceNodeType>::iterator res = std::min_element(
        cache_.begin(), cache_.end(), [&](auto& pnode, auto& ppnode) {
          return std::accumulate(pnode.begin(), pnode.end(), 0, acc) <
                 std::accumulate(ppnode.begin(), ppnode.end(), 0, acc);
        });

    if (res == cache_.end()) {
      res = cache_.begin();
    }

    res->clear();
    hash_table_.erase(std::find_if(hash_table_.begin(), hash_table_.end(),
                                   [&](auto& el) { return el.second == res; }));
    return res;
  }

  std::vector<PieceNodeType>::iterator GetEmptySlot() {
    if (hash_table_.size() == cache_.size())
      return SwapOut();
    return std::find_if(cache_.begin(), cache_.end(),
                        [](auto& el) { return el.empty(); });
  }

  PieceNodeType::iterator FetchPieceFromDisk(size_t index,
                                             size_t length = 16 * 1024) {
    // this class didn't check that is piece in here,
    // this task privoded by uppper logic
    // only if piece has been downloaded, Get() call will be called.
    AbsFileManager* fmanager_ = FilesManagerSV::GetInstance();
    ASSERT_NOT_NULPTR(fmanager_);

    std::vector<PieceNodeType>::iterator slots = GetEmptySlot();
    for (int i = 0; i < 16; ++i) {
      Node node(index, i * 16 * 1024);
      *fmanager_ >> node;
    }
  }

  void Refer(Node new_data) {
    std::vector<PieceNodeType>::iterator slots;
    if (hash_table_.find(new_data.index) == hash_table_.end()) {
      slots = GetEmptySlot();
      slots->push_back(std::move(new_data));
      hash_table_[new_data.index] = slots;
    } else {
      //如果piece已经在cache中
      // 则查找offset是够也在
      slots = hash_table_.at(new_data.index);
      PieceNodeType::iterator slice_slots =
          std::find_if(slots->begin(), slots->end(),
                       [&](auto& el) { return el.offset == new_data.offset; });

      if (slice_slots == slots->end()) {
        slots->push_back(std::move(new_data));
      }
    }
  }

  void Refer(std::vector<Node> new_data) {
    if (new_data.empty())
      return;
    std::vector<PieceNodeType>::iterator slots;
    if (hash_table_.find(new_data[0].index) == hash_table_.end()) {
      slots = GetEmptySlot();
      *slots = (std::move(new_data));
      hash_table_[new_data[0].index] = slots;
    } else {
      // nothing to do;
    }
  }

  std::shared_ptr<Node> Get(size_t index, size_t offset) {
    // 1. 查询这个piece是够本地已经下载好了
    // 2. 如果有则查询是否在cache中？
    // 3. 在本地就直接返回该node，如果没有则执行4
    // 4. 初始化容器node，然后向文件中读取该完整piece；然后返回指定node；

    if (hash_table_.find(index) == hash_table_.end()) {
      // cache中没有对应的piece
      FetchPieceFromDisk(index);
    } else {
      auto& nodes = *hash_table_[index];
      std::vector<Node>::iterator node =
          std::find_if(nodes.begin(), nodes.end(),
                       [&](auto& el) { return el.offset == offset; });
      if (node == nodes.end()) {
        return nullptr;
      }
      return std::make_shared<Node>(*node);
    }

    return nullptr;
  }

  size_t Size() { return hash_table_.size(); }

 public:
  // for singleton design;
};

#endif  // BITUSK_SRC_DATACACHE_H__
