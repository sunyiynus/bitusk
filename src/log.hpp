#ifndef BITUSK_SRC_LOG_H
#define BITUSK_SRC_LOG_H

#include <boost/smart_ptr/shared_ptr.hpp>
#include <istream>
#include <ostream>
#include <sstream>

#include "basic.hpp"

class Logger {
 public:
  static Logger& Instance();

 public:
  enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FATAL };

  Logger() = default;
  Logger(const Logger& lg);
  Logger& Init(std::ostream* ffs);

  Logger& log(LogLevel lv, const std::string& str);
  const std::string LevelStr(LogLevel lv);
  const std::string TimeStr();

 private:
#ifdef DEBUG_MACRO
  std::ostream* fs;
#else
  std::shared_ptr<std::ostream> fs;
#endif
};

class LoggerStream {
 public:
  LoggerStream() = delete;
  LoggerStream(const LoggerStream& lg);
  LoggerStream(Logger& lg);
  LoggerStream(Logger& lg, Logger::LogLevel lv);
  ~LoggerStream();

  LoggerStream Trace();
  LoggerStream Debug();
  LoggerStream Info();
  LoggerStream Warn();
  LoggerStream Error();
  LoggerStream Fatal();

  template <typename T>
  LoggerStream& operator<<(const T& el) {
    ostr << el;
    return *this;
  }

 private:
  Logger& logger;
  Logger::LogLevel lv;
  std::ostringstream ostr;
};

#endif
