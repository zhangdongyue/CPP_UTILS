/*======================================================
 * Copyright (c) 2015-2016 ZIPPY.ZDY All Rights Reserved.
 *======================================================*/
#ifndef SRC_UTILS_THIRFT_SERIALIZE_H_
#define SRC_UTILS_THIRFT_SERIALIZE_H_

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <thrift/transport/TBufferTransports.h>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <memory>
#include <exception>
#include <string>

// @Author Dongyue.Zhang
// @Mailto zhangdy1986(at)gmail.com 
// @Date 2016-09-08 17:30:00
// @Brief serialize/deserialize thrift structure.

namespace utils {

#define LOG_EXCEPTIONG(msg) LOG(ERROR) << __func__ << " THROW EXCEPTION:" << msg

using apache::thrift::transport::TMemoryBuffer;
using apache::thrift::protocol::TBinaryProtocol;

template <typename T, typename Tprotocol>
class ThriftSerialize {
 public:
    ThriftSerialize():
      _transport(nullptr),
      _protocol(nullptr) {}
    virtual ~ThriftSerialize() {}

    bool Init() noexcept;

    bool Encode(const T& t, std::string& obuffer) noexcept;

    std::shared_ptr<T> Decode(uint8_t * data, uint32_t len) noexcept;

    void BufferClear();

 private:
    boost::shared_ptr<TMemoryBuffer> _transport;
    boost::shared_ptr<Tprotocol> _protocol;
};

template <typename T, typename Tprotocol>
bool ThriftSerialize<T, Tprotocol>::Init() noexcept {
  try {
    _transport = boost::make_shared<TMemoryBuffer>();
    _protocol = boost::make_shared<Tprotocol>(_transport);
    return true;
  } catch (std::exception& e) {
    LOG_EXCEPTIONG(e.what());
    return false;
  } catch (...) {
    LOG_EXCEPTIONG("Unkown");
    return false;
  }
}

template <typename T, typename Tprotocol>
void ThriftSerialize<T, Tprotocol>::BufferClear() {
  _transport->resetBuffer();
}

template <typename T, typename Tprotocol>
bool ThriftSerialize<T, Tprotocol>::Encode(
    const T& t, std::string& obuffer) noexcept {
  try {
    // _transport->resetBuffer();
    t.write(_protocol.get());
    obuffer = _transport->getBufferAsString();
    return true;
  } catch (std::exception& e) {
    LOG_EXCEPTIONG(e.what());
    return false;
  } catch (...) {
    LOG_EXCEPTIONG("Unkown");
    return false;
  }
}

template <typename T, typename Tprotocol>
std::shared_ptr<T> ThriftSerialize<T, Tprotocol>::Decode(
    uint8_t *data, uint32_t len) noexcept {
  try {
    std::shared_ptr<T> p_clst = std::make_shared<T>();
    _transport->resetBuffer(data, len);
    p_clst->read(_protocol.get());
    return p_clst;
  } catch (std::exception& e) {
    LOG_EXCEPTIONG(e.what());
    return nullptr;
  } catch (...) {
    LOG_EXCEPTIONG("Unkown");
    return nullptr;
  }
}

}  // namespace utils

#endif  // SRC_UTILS_THIRFT_SERIALIZE_H_
