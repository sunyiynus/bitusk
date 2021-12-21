
#include "datacache.hpp"


/*

Cache::Cache(std::iostream& fstream): fstream_(fstream), piece_limit_(1024), slice_size_(16) {

}



// For User using API
Cache& Cache::Write(std::pair<size_t, size_t> pos, std::shared_ptr<SliceNodeType> node) {
    
}


template<typename T>
Cache& Cache::Write(size_t piece_index, typename T::iterator beg, typename T::iterator end) {

}

SliceNodeType Cache::Read(size_t piece_index, size_t slice_offset) {

}


std::vector<SliceNodeType> Cache::Read(size_t piece_index) {

}


// Utilities
//
bool Cache::IsInCache(size_t piece_index) {
    if( cache_.find(piece_index) == cache_.end() ) {
        return false;
    } else {
        return true;
    }
}


bool Cache::IsInCache(size_t piece_index, size_t slice_offset) {
    if(IsInCache(piece_index)) {
        return cache_[piece_index].second[slice_offset].get();
    } else {
        return false;
    }
} 


void Cache::ReadFromDisk(size_t piece_index, size_t slice_offset) {
    if( IsInCache(piece_index, slice_offset) ) return;
    // this class just take care of write and read piece from disk or to disk

    std::shared_ptr<SliceNodeType> buffer;
    char tmp;
    fstream_.seekg( slice_offset * 1024 + piece_index *1024 * 16, std::ios::beg);
    for( size_t i = 0; i < 1024 && fstream_.get(tmp); ++i ) {
        buffer->push_back(tmp);
    }

    cache_[piece_index].second[slice_offset] = buffer;
    
    cache_[piece_index].first.seond

}


void Cache::ReadFromDisk(size_t piece_index) {
    if( IsInCache(piece_index) ) {

    }
    
}

*/


