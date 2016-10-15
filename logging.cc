/*====================================================
 * Copyright (c) 2015-2016 ZIPPY.Z All Rights Reserved.
 *====================================================*/
#include "utils/logging.h"

// @Author dongyue.zhang(zhangdy1986(at)gmail.com)
// @Brief Logging print interface

namespace utils {

int32_t FileLogPolicy::_kMainThreadPid = getpid();
int32_t FileLogPolicy::_kMainDay = 0;

bool FileLogPolicy::DayHasChanged(time_t timestamp) {
  int32_t days = (timestamp + 28800)/86400;
  if (days != _kMainDay) {
    _kMainDay = days;
    return true;
  }
  return false;
}

bool FileLogPolicy::PidHasChanged() {
  int32_t pid = getpid();
  if (_kMainThreadPid == pid) {
    return false;
  }
  _kMainDay = pid;
  return true;
}

std::string FileLogPolicy::GetNameExtStr() const {
  time_t timestamp = time(NULL);
  struct ::tm tm_time;
  localtime_r(&timestamp, &tm_time);

  std::ostringstream time_pid_stream;
  time_pid_stream.fill('0');
  time_pid_stream << 1900+tm_time.tm_year
    << std::setw(2) << 1+tm_time.tm_mon
    << std::setw(2) << tm_time.tm_mday
    << '-'
    << std::setw(2) << tm_time.tm_hour
    << std::setw(2) << tm_time.tm_min
    << std::setw(2) << tm_time.tm_sec
    << '.'
    << GetMainThreadPid();

  return time_pid_stream.str();
}

void FileLogPolicy::OpenOstream(const std::string& name) {
  _file_len = 0;
  _log_name_prefix = name;
#if 0
  std::string filename = name + GetNameExtStr();
  _out_stream->open(filename.c_str(),
      std::ios_base::binary | std::ios_base::out);
  if (!_out_stream->is_open()) {
    throw(std::runtime_error("LOGGER:Unable to open an output stream"));
  }
#endif
}

void FileLogPolicy::OpenOstream() {
  _file_len = 0;
  std::string filename = _log_name_prefix + GetNameExtStr();
  _out_stream->open(filename.c_str(),
      std::ios_base::binary | std::ios_base::out);
  if (!_out_stream->is_open()) {
    throw(std::runtime_error("LOGGER:Unable to open an output stream"));
  }
}

void FileLogPolicy::CloseOstream() {
  if (_out_stream) {
    _out_stream->close();
  }
}

void FileLogPolicy::Write(const std::string& msg, time_t timestamp) {
  _file_len += msg.length();
  if (_file_len >> 20 > _max_file_len || DayHasChanged(timestamp) ||
      PidHasChanged()) {
    CloseOstream();
    OpenOstream();
  }
  (*_out_stream) << msg << std::endl;
  // Write(msg.data(), msg.length(), timestamp);
}

void FileLogPolicy::Write(
    const char* data, size_t size, time_t timestamp) {
  _file_len += size;
  if (_file_len >> 20 > _max_file_len || DayHasChanged(timestamp) ||
      PidHasChanged()) {
    _out_stream->flush();
    CloseOstream();
    OpenOstream();
  }
  _out_stream->write(data, size);
}

FileLogPolicy::~FileLogPolicy() {
  if (_out_stream) {
    CloseOstream();
  }
}

}  // namespace utils
