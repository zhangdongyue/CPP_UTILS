/*====================================================
 * Copyright (c) 2015-2016 ZIPPY.Z All Rights Reserved.
 *====================================================*/
#ifndef SRC_UTILS_LOGGING_H_
#define SRC_UTILS_LOGGING_H_
#include<time.h>
#include<unistd.h>

#include<string>
#include<mutex>
#include<sstream>
#include<fstream>
#include<memory>
#include<iostream>
#include<iomanip>
#include<atomic>

// @Author dongyue.zhang(zhangdy1986(at)gmail.com)
// @Brief Log print, thread safe, TODO: (zdy)no buffering

namespace utils {
enum  SeverityType{
  debug = 1,
  error,
  warning,
  binary,
};

template<typename TLogPolicy>
class Logger {
 public:
    explicit Logger(const std::string& name);
    virtual ~Logger() noexcept;

    template<SeverityType severity, typename...Args>
    void Print(Args...args);

    void PrintBinary(const char* data, size_t len);

    void SetMaxFileLen(int32_t len);

 private:
    std::string GetTime();
    std::string GetLogingHeader();
    void PrintImpl();

    template<typename TFirst, typename...TRest>
    void PrintImpl(TFirst first, TRest...rest);

 private:
    std::atomic<unsigned> _log_ling_number;
    TLogPolicy* _policy;
    std::mutex _write_mutex;
    std::stringstream _log_stream;
    time_t _timestamp;
};

template<typename TLogPolicy>
Logger<TLogPolicy>::Logger(const std::string& name) {
  _timestamp = time(NULL);
  _log_ling_number = 0;
  _policy = new(std::nothrow) TLogPolicy();
  if (!_policy) {
    throw std::runtime_error("LOGGER:Unable to create the logger instance");
  }
  _policy->OpenOstream(name);
}

template<typename TLogPolicy>
Logger<TLogPolicy>::~Logger() noexcept {
  if (_policy) {
    _policy->CloseOstream();
    delete _policy;
  }
}

template<typename TLogPolicy>
void Logger<TLogPolicy>::SetMaxFileLen(int32_t len) {
  _policy->SetMaxFileLen(len);
}

template<typename TLogPolicy>
template<SeverityType severity, typename...Args>
void Logger<TLogPolicy>::Print(Args...args) {
  // TODO(dongyue): Mutex is slower,add buffering or shared mem
  _write_mutex.lock();
  switch (severity) {
    case SeverityType::debug:
      _log_stream << "[DEBUG]\t";
      break;
    case SeverityType::error:
      _log_stream << "[ERROR]\t";
      break;
    case SeverityType::warning:
      _log_stream << "[WARNING]\t";
  }
  PrintImpl(args...);
  _write_mutex.unlock();
}

template<typename TLogPolicy>
void Logger<TLogPolicy>::PrintBinary(
    const char* data, size_t len) {
  _write_mutex.lock();
  _timestamp = time(NULL);
  _policy->Write(data, len, _timestamp);
  _write_mutex.unlock();
}

template<typename TLogPolicy>
std::string Logger<TLogPolicy>::GetTime() {
#if 0
  std::string time_str;
  time_t raw_time;

  time(&raw_time);
  time_str = ctime_r(&raw_time);
  return time_str.substr(0, time_str.size() - 1);
#endif
  _timestamp = time(NULL);
  struct ::tm tm_time;
  localtime_r(&_timestamp, &tm_time);

  std::ostringstream time_stream;
  time_stream.fill('0');
  time_stream << 1900+tm_time.tm_year
    << std::setw(2) << 1+tm_time.tm_mon
    << std::setw(2) << tm_time.tm_mday
    << '-'
    << std::setw(2) << tm_time.tm_hour
    << std::setw(2) << tm_time.tm_min
    << std::setw(2) << tm_time.tm_sec;

  return time_stream.str();
}

template<typename TLogPolicy>
std::string Logger<TLogPolicy>::GetLogingHeader() {
  std::stringstream header;

  header.str("");
  header.fill('0');
  header.width(7);
  header << _log_ling_number++ << "\t[" << GetTime() << "-";

  header.fill('0');
  header.width(7);
  header << clock() << "]\t";

  return header.str();
}

template<typename TLogPolicy>
void Logger<TLogPolicy>::PrintImpl() {
  _policy->Write(GetLogingHeader() + _log_stream.str(), _timestamp);
  _log_stream.str("");
}

template<typename TLogPolicy>
template<typename TFirst, typename...TRest>
void Logger<TLogPolicy>::PrintImpl(TFirst first, TRest...rest) {
  _log_stream << first;
  PrintImpl(rest...);
}

/* Core log object */
class LogPolicyInterface {
 public:
    virtual void OpenOstream(const std::string& name) = 0;
    virtual void CloseOstream() = 0;
    virtual void Write(const std::string& msg,
        time_t timestamp) = 0;
    virtual void Write(const char* data,
        size_t size, time_t timestamp) = 0;

    virtual void SetMaxFileLen(int32_t len) = 0;
};

class FileLogPolicy : public LogPolicyInterface {
 public:
    FileLogPolicy():
      _out_stream(new std::ofstream),
      _max_file_len(1800),
      _log_name_prefix("") {}

    void OpenOstream(const std::string& name);
    void CloseOstream();

    void Write(const std::string& msg, time_t timestamp);
    void Write(const char* data, size_t size, time_t timestamp);

    virtual ~FileLogPolicy();

    void SetMaxFileLen(int32_t len) {
      _max_file_len = len;
    }

 private:
    void OpenOstream();
    bool DayHasChanged(time_t timestamp);
    bool PidHasChanged();
    int32_t GetMainThreadPid() const {
      return _kMainThreadPid;
    }
    std::string GetNameExtStr() const;

 private:
    std::unique_ptr<std::ofstream> _out_stream;
    std::string _log_name_prefix;
    int32_t _file_len; /*Bytes*/
    int32_t _max_file_len; /*MBytes*/
    static int32_t _kMainThreadPid;
    static int32_t _kMainDay;
};

}  // namespace utils

#endif  // SRC_UTILS_LOGGING_H_

/* vim: set ts=2 sts=2 sw=2 tw=80 et */
