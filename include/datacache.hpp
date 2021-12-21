#ifndef BITUSK_SRC_DATACACHE_H__
#define BITUSK_SRC_DATACACHE_H__

#include <algorithm>
#include <bits/types/FILE.h>
#include <boost/circular_buffer.hpp>

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <map>
#include <vector>
#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <utility>
#include <numeric>
#include <functional>
#include <cassert>
#include <fstream>

#include "peer.hpp"
#include "metafileparser.hpp"

//  :
//  |------
//  |------
//  |------
//  :

typedef std::basic_string<unsigned char> ustring;
struct Node{
    Node():index(0), offset(0),in_use(false), write_read(true),
                        is_writed(true), access_count(0) {}
    Node(int i, int o): index(i), offset(o), 
                        in_use(true), write_read(true),
                        is_writed(true), access_count(0) {}

    Node(const Node& n) = default;
    Node(Node&& n) = default;
    Node& operator=(const Node& n) = default;
    ustring buffer;
    int index;
    int offset;

    bool in_use;
    bool write_read;
    bool is_writed;

    bool isFull() const {
        return (buffer.size() == 16 * 1024) ? true:false;
    }

    int access_count;
    Node& operator++() {
        access_count++;
        return *this;
    }

    Node operator++(int i) {
        this->operator++();
        return *this;
    }

    Node& Write(std::ostream& os){
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

    operator bool() {
        return in_use;
    }

/*
    bool operator==(const Node& rhl) {
        if( this == &rhl )
        return in_use == node.in_use;
    }

    bool operator==(const bool v) {
        return in_use == v;
    }
*/
};

/*
std::ostream& operator<<(std::ostream& os, Node& node) {
    node.Write(os);
    return os;
}

std::istream& operator>>(std::istream& is, Node& node) {
    node.Read(is);
    return is;
}
*/


template<typename T>
int sum_over_vector(const std::vector<T>& v, const std::function<int(const T&)>& member ) {
    return std::accumulate(v.begin(), v.end(), 0, [&](int sum, const T& curr) { return sum + member(curr); });
}


class FilesManager {
private:

    std::vector<std::pair<long,std::shared_ptr<std::fstream>>> files_;
    
    Peer* myself_;

    //std::vector<bitusk::FileNode> files_info_;
    std::string download_dir_;

public:

    FilesManager() = default;
    FilesManager(const std::string& download_dir):download_dir_(download_dir){}

    bool CreateFile(const std::vector<bitusk::FileNode>& files) {
        for( auto& file: files ) {
            std::string file_path = download_dir_ + '/' + file.path;
            std::shared_ptr<std::fstream> fps = 
                    std::make_shared<std::fstream>(file_path, std::ios::ate|std::ios::binary);
            files_.push_back(std::make_pair(file.length,fps));
        }
        assert(files.size() == files_.size());
        return true;
    }

    // just read didn't judge;
    // 检查是否有piece已经下载，就不让这个class负责
    // TODO download cache 判断一个piece是够下载完
    void ReadFromDisk(Node& node) {
        assert( files_.empty() == false );
        
    }

    void WriteToDisk(Node& node) {

        //把piece index 看做一个stream，挨个写，总长就是各文件相加之和
        //if( files_.empty()) return;
        assert( files_.empty() == false );
        long long line_pos = node.index * 256 * 1024 + node.offset;

        if( files_.size() == 1) {
            std::fstream& fos = *(files_[0].second);
            fos.seekp(line_pos);

            //std::string tmpbuf (node.buffer.begin(), node.buffer.end());

            fos << reinterpret_cast<const char*>(node.buffer.c_str());
            //fos << tmpbuf;
        }


        for( auto itr = files_.begin(); itr != files_.end(); ) {
            auto& fpair = *itr;
            if( (line_pos < fpair.first) && ( (line_pos + node.buffer.size()) < fpair.first)) {
                //node中的内容属于一个文件
                std::fstream& fos = *(fpair.second);
                fos << reinterpret_cast<const char*>(node.buffer.c_str());
                break;
            } else if( (line_pos < fpair.first) && ((line_pos + node.buffer.size()) >= fpair.first)) {
                //node中的内容需要多个文件
                ustring::size_type writed_offset = 0;
                ustring::size_type left = node.buffer.size();

                std::fstream& fos = *(fpair.second);
                fos.seekp(line_pos);
                ustring tmpbuffer (node.buffer.begin(), node.buffer.begin() + fpair.first - line_pos);
                fos << reinterpret_cast<const char*>(tmpbuffer.c_str());
                node.buffer.erase(node.buffer.begin(), node.buffer.begin() + fpair.first - line_pos);
                itr++;

                // 3个文件+的情况
                while( !node.buffer.empty() ) {
                    if( itr->first >= node.buffer.size() ) {
                        std::fstream& fos = *(itr->second);
                        fos.seekp(0);
                        fos << reinterpret_cast<const char*>(node.buffer.c_str());
                        node.buffer.erase(node.buffer.begin(), node.buffer.end());
                    } else {
                        std::fstream& fos = *(itr->second);
                        fos.seekp(0);
                        tmpbuffer.clear();
                        tmpbuffer.assign(node.buffer.begin(), node.buffer.begin() + itr->first);
                        fos << reinterpret_cast<const char*>(tmpbuffer.c_str());
                        node.buffer.erase(node.buffer.begin(), node.buffer.begin() + itr->first);
                        itr++;
                    }
                }
                break;
            } else {
                line_pos -= itr->first;
                itr++;
            }

        }
        return;
    }

    const size_t Size() const {
        return files_.size();

    }
};



inline FilesManager& operator<<(FilesManager& os, Node& node) {
    os.WriteToDisk(node);
    return os;
}

inline FilesManager& operator>>(FilesManager& is, Node& node) {
    is.ReadFromDisk(node);
    return is;
}


struct PieceNode{
    std::vector<Node> slices;

public:
    PieceNode():slices(16, Node()) {}
    const bool IsCompletePiece() {
        slices.erase(std::unique(slices.begin(), slices.end(), [](auto& a, auto& b) {
                return a.offset == b.offset; }), slices.end());
        std::sort(slices.begin(), slices.end(), [](auto& a, auto& b) {
                return a.offset < b.offset; });

        return true;
    }

    void Add(Node& node) {
        slices.push_back(node);
    }
};

inline PieceNode& operator+=(PieceNode& pnode, Node& node) {
    pnode.Add(node);
    return pnode;
}



class CacheV1{
private:
    typedef std::vector<Node> PieceNodeType;
    
    std::vector<PieceNodeType> cache_;
    std::map<size_t, std::vector<PieceNodeType>::iterator> hash_table_;
    FilesManager* fmanager_;
    Peer* myself_;
public:
    // some other operation

    void SetFileManager(FilesManager* fm) {
        if( ! fm ) return;
        fmanager_ = fm;
    }

    void SetMyselfPeer(Peer* myself) {
        if( !myself ) return;
        myself_ = myself;
    }

public:
    CacheV1(): cache_(64, std::vector<Node>()) {}
    CacheV1(size_t ps, size_t ss): cache_(ps, std::vector<Node>()),
                                    fmanager_(nullptr), myself_(nullptr) {}

    std::vector<PieceNodeType>::iterator SwapOut() {
        // 1. 找到访问次数最少的Piece node
        // 2. 更改此piece node中所有 slice node的in use flag
        // 3. 在hash table中删除该piece node的记录
        // 4. 返回该piece node 的itr
        auto acc = [&](int sum, const Node& node){ return sum + node.access_count; };

        std::vector<PieceNodeType>::iterator res = std::min_element(cache_.begin(), cache_.end(), 
        [&](auto& pnode, auto& ppnode) {
                return std::accumulate(pnode.begin(), pnode.end(), 0, acc)
                        < std::accumulate( ppnode.begin(), ppnode.end(), 0, acc); });

        if( res == cache_.end() ) {
            res = cache_.begin();
        }

        res->clear();
        hash_table_.erase( std::find_if(hash_table_.begin(), hash_table_.end(), 
                            [&](auto& el) { return el.second == res; }));
        return res;
    }

    std::vector<PieceNodeType>::iterator GetEmptySlot() {
        if( hash_table_.size() == cache_.size() ) return SwapOut();
        return std::find_if(cache_.begin(), cache_.end(), [](auto& el) { return el.empty(); });
    }

    PieceNodeType::iterator FetchPieceFromDisk(size_t index, size_t length = 16 * 1024) {
        // this class didn't check that is piece in here,
        // this task privoded by uppper logic
        // only if piece has been downloaded, Get() call will be called.
        assert( fmanager_ != nullptr );

        std::vector<PieceNodeType>::iterator slots = GetEmptySlot();
        for( int i = 0; i < 16; ++i){
            Node node(index, i * 16 * 1024);
            *fmanager_ >> node;
        }
    }

    void Refer(Node new_data) {
        std::vector<PieceNodeType>::iterator slots;
        if( hash_table_.find(new_data.index) == hash_table_.end() ) {
            slots = GetEmptySlot();
            slots->push_back(std::move(new_data));
            hash_table_[new_data.index] = slots;
        } else {
            //如果piece已经在cache中
            // 则查找offset是够也在
            slots = hash_table_.at(new_data.index);
            PieceNodeType::iterator slice_slots = std::find_if(slots->begin(), slots->end(),
                                [&](auto& el){ return el.offset == new_data.offset; });

            if( slice_slots ==  slots->end() ) {
                slots->push_back(std::move(new_data));
            }
        }
    }

    void Refer(std::vector<Node> new_data) {
        if( new_data.empty()) return;
        std::vector<PieceNodeType>::iterator slots;
        if( hash_table_.find(new_data[0].index) == hash_table_.end() ) {
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

        if( hash_table_.find(index) == hash_table_.end() ) {
            // cache中没有对应的piece
            FetchPieceFromDisk(index);
        } else {
            auto& nodes = *hash_table_[index];
            std::vector<Node>::iterator node = std::find_if(nodes.begin(), nodes.end(), 
                    [&](auto& el){ return el.offset == offset; });
            if( node == nodes.end() ) {
                return nullptr;
            }
            return std::make_shared<Node>(*node);
        }

        return nullptr;
    }


    size_t Size() {
        return hash_table_.size();
    }



};


/*
// 这些代码是另外一种写法 很可惜失败了
template <typename IT, typename DT, size_t N>
class ThreadSafeCache{
private:
    typedef std::pair<IT,DT> DataType;
    struct node{
        std::shared_ptr<DataType> data;
        std::unique_ptr<node> next;
    };

    std::shared_ptr<node> head;
    node* tail;
public:
    std::list<std::shared_ptr<DataType>> try_read(size_t piece_index, size_t slice_offset);

    std::list<std::shared_ptr<DataType>> wait_read(size_t piece_index, size_t slice_offset);

    void try_write(const DataType& new_data);
    void try_write(DataType&& new_data);

    void waite_write(const DataType& new_data);
    void waite_write(DataType&& new_data);




    
};

class SwapStrategy {

};

template<typename IT, typename DT, typename STRATGY = SwapStrategy >
class DataCache {
private:

    typedef std::pair<IT,std::shared_ptr<DT>> DataType;
    typedef std::shared_ptr<DataType> DataPtrType;


   // std::map<size_t,DataPtrType> cache_;
   //
    std::vector<DataType> cache;
    std::map<size_t, typename std::vector<DataType>::iterator> index_;

    STRATGY strategy;

public:


    void Write(DataType new_data);
    void Write(IT info, DT new_data);

    template<typename Funtional>
    std::shared_ptr<DT> Read(size_t index, Funtional func);

};

*/

/*
struct PieceState;

class AbsSwapPolicy {
public:
    
};



class Cache {
public:
    Cache() = delete;
    Cache(const Cache& cache) = delete;
    Cache& operator=(const Cache& cache) = delete;
    Cache(std::iostream& iostream);
    Cache(Cache&& cache);
    ~Cache();

    Cache(size_t piece_limit, size_t slice_n);
    typedef std::vector<unsigned char> SliceNodeType;
    typedef std::map<size_t, std::pair<PieceState, std::vector<std::shared_ptr<SliceNodeType>>>> CacheType;

    // Public to user write and read from cache

    Cache& Write(std::pair<size_t,size_t> pos, std::shared_ptr<SliceNodeType> node);
    template<typename T>
    Cache& Write(size_t piece_index, typename T::iterator beg, typename T::iterator end);
    
    SliceNodeType            Read(size_t piece_index, size_t slice_offset);
    std::vector<SliceNodeType> Read(size_t piece_index);
public: //  utilities
    bool IsInCache(size_t piece_index);
    bool IsInCache(size_t piece_index, size_t slice_offset);


private:
    // class self owned, write or read from DISK
    void ReadFromDisk(size_t piece_index, size_t slice_offset);
    void ReadFromDisk(size_t piece_index);

    void WriteToDisk(size_t piece_index, size_t slice_offset);
    void WriteToDisk(size_t piece_index);

private:
    CacheType cache_;
    // Cache capbilty
    size_t piece_limit_;
    size_t slice_size_;
    std::iostream& iostream_;
};

*/


#endif // BITUSK_SRC_DATACACHE_H__

