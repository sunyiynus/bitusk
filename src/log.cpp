#include "log.hpp"

#include <sstream>
#include <iostream>
#include <chrono>
#include <string>
#include <cassert>
#include <memory>
#include <ctime>


Logger& Logger::Instance() {
    static Logger logger;
    return logger;
}


Logger& Logger::Init(std::ostream *ffs) {
    assert( ffs != nullptr );
#ifdef DEBUG_MACRO
    fs = ffs;
#else
    fs.reset(ffs);
#endif
    return *this;
}


Logger& Logger::log(LogLevel lv, const std::string &str) {
    std::string nstr (str);
    if( str.find('\n') == std::string::npos) {
        nstr += "\n";
    }
    *fs << TimeStr() << " [" << LevelStr(lv) << "] " << nstr;
    return *this;
}

const std::string Logger::LevelStr(LogLevel lv) {
    std::string result;
    switch(lv) {
        case LogLevel::TRACE:
            result = "Trace";
            break;
        case LogLevel::DEBUG: 
            result = "Debug";
            break;
        case LogLevel::INFO:
            result = "Information";
            break;
        case LogLevel::WARN:
            result = "Warning";
            break;
        case LogLevel::ERROR:
            result = "Error";
            break;
        case LogLevel::FATAL:
            result = "Fatal";
            break;
        default:
            result = "Trace";
            break;
    }
    return result;
}

const std::string Logger::TimeStr() {
    std::ostringstream ostr;
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
    time_t t = std::chrono::system_clock::to_time_t(tp);
    std::string ts = std::ctime(&t);
    return ts.substr(0, ts.size()-1);
}


LoggerStream::LoggerStream(const LoggerStream& lg):logger(lg.logger), lv(lg.lv) {}
LoggerStream::LoggerStream(Logger& lg): logger(lg) {}
LoggerStream::LoggerStream(Logger& lg, Logger::LogLevel lvv): logger(lg), lv(lvv) {}


LoggerStream LoggerStream::Trace() {
    lv = Logger::LogLevel::TRACE;
    return LoggerStream(*this);
}


LoggerStream LoggerStream::Debug() {
    lv = Logger::LogLevel::DEBUG;
    return LoggerStream(*this);
}


LoggerStream LoggerStream::Info() {
    lv = Logger::LogLevel::INFO;
    return LoggerStream(*this);
}


LoggerStream LoggerStream::Warn() {
    lv = Logger::LogLevel::WARN;
    return LoggerStream(*this);
}


LoggerStream LoggerStream::Error() {
    lv = Logger::LogLevel::ERROR;
    return LoggerStream(*this);
}


LoggerStream LoggerStream::Fatal() {
    lv = Logger::LogLevel::FATAL;
    return LoggerStream(*this);
}


LoggerStream::~LoggerStream() {
    if( !std::string(ostr.str()).empty())
        logger.log(lv, ostr.str());
}