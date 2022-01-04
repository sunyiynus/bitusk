
#include "datacache.hpp"

#include <atomic>
#include <cstddef>
#include <mutex>
#include <numeric>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <memory>




#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L )
#else
std::atomic<FilesManagerSV*> FilesManagerSV::mfm_instance;
std::mutex FilesManagerSV::m_mutex;
#endif

FilesManagerSV* FilesManagerSV::GetInstance(const std::string &dd, const std::vector<File> &file) {
    FilesManagerSV* tmp =  mfm_instance.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if( nullptr == tmp ) {
        std::lock_guard<std::mutex> lok(m_mutex);
        if( nullptr == tmp ) {
            tmp = new FilesManagerSV(dd, file);
            std::atomic_thread_fence(std::memory_order_acquire);
            mfm_instance.store(tmp, std::memory_order_relaxed);
        }
    }
    return tmp;
}


FilesManagerSV* FilesManagerSV::GetInstance() {
    FilesManagerSV* tmp =  mfm_instance.load(std::memory_order_relaxed);
    return tmp;
}


FilesManagerSV::FilesManagerSV(const std::string& dd, const std::vector<File>& files):
    AbsFileManager(dd, files) {
        total_length_ = 0;
        for( auto& file: files) {
            total_length_ += file.length;
        }
}


bool FilesManagerSV::LoadingFiles() {
    if( files_info_.empty() ) return false;

    if( fs_.get() ) {
        return true;
    }

    fs_ = std::make_shared<std::fstream>(download_dir_ + files_info_[0].path + ".tmp",
                std::ios::in|std::ios::out|std::ios::app|std::ios::binary);
    if( !(*fs_) ) {
        fs_.reset((std::iostream*)nullptr);
        return false;
    }
    return true;
}


void FilesManagerSV::ReadSliceNodeFromDisk(Node& node) {
    if ( !IsFileReady() ) return;

    std::shared_ptr<char> buffer ( new char[node.buffer.size()],
                                 [](char *p) { delete [] p;});
    fs_->seekg(node.index * 256 * 1024 + node.offset );
    if( !(*fs_) ) {
        node.buffer.clear();
    }
    size_t bytes = fs_->readsome(buffer.get(), node.buffer.size());
    if( bytes > 0 ) {
        node.buffer = ustring(reinterpret_cast<unsigned char*>(buffer.get()), node.buffer.size());
    } else {
        node.buffer.clear();
    }
}


const Node FilesManagerSV::ReadSliceNodeFromDisk(size_t index, size_t offset, size_t length) {
    Node node(index, offset);
    node.buffer = ustring(length, ' ');
    if ( !IsFileReady() ) return node;
    ReadSliceNodeFromDisk(node);
    return node;
}


bool FilesManagerSV::WriteSliceNodeToDisk(const Node& node) {
    if ( !IsFileReady() ) return false;

    fs_->seekp(node.index * 256 * 1024 + node.offset, std::ios::beg);
    fs_->write(reinterpret_cast<const char*>(node.buffer.c_str()), node.buffer.size());
    fs_->flush();
    return true;
}


bool FilesManagerSV::IntegrateFiles() {
    // TODO
    return true;
}


const bool FilesManagerSV::IsFileReady() const {
    
}


// class FilesManagerDecorator

FilesManagerDecorator& FilesManagerDecorator::GetInstance() {
    static FilesManagerDecorator fmd (FilesManagerSV::GetInstance());
    return fmd;
}



bool FilesManagerDecorator::LoadingFiles() {
    ASSERT_NOT_NULPTR(fmanager_);
    return fmanager_->LoadingFiles();
}


void FilesManagerDecorator::ReadSliceNodeFromDisk(Node& node) {
    ASSERT_NOT_NULPTR(fmanager_);
    return fmanager_->ReadSliceNodeFromDisk(node);
}


const Node FilesManagerDecorator::ReadSliceNodeFromDisk(const size_t index, const size_t offset, const size_t length) {
    ASSERT_NOT_NULPTR(fmanager_);
    return fmanager_->ReadSliceNodeFromDisk(index, offset, length);
}


bool FilesManagerDecorator::WriteSliceNodeToDisk(const Node& node) {
    ASSERT_NOT_NULPTR(fmanager_);
    return fmanager_->WriteSliceNodeToDisk(node);
}


bool FilesManagerDecorator::IntegrateFiles() {
    ASSERT_NOT_NULPTR(fmanager_);
    return fmanager_->IntegrateFiles();
}


const PieceNode FilesManagerDecorator::ReadPieceFromDisk(const size_t index, const size_t piece_n) {
    ASSERT_NOT_NULPTR(fmanager_);
    PieceNode pnode;
    for( size_t i = 0; i < piece_n; ++i ) {
        Node node(index, i * 16 * 1024);
        pnode += node;
    }
    return pnode;
}


bool FilesManagerDecorator::WritePieceToDisk(const PieceNode& piece) {
    ASSERT_NOT_NULPTR(fmanager_);
    const auto& slice = piece.Get();
    for( const auto& node:slice ) {
        fmanager_->WriteSliceNodeToDisk(node);
    }
    return true;
}




