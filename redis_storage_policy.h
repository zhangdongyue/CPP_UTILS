//=====================================================
// Copyright(c) 2015-2017 ZIPPY.ZDY All Rights Reserved.
//=====================================================
#ifndef SRC_UTILS_REDIS_STORAGE_POLICY_H_
#define SRC_UTILS_REDIS_STORAGE_POLICY_H_

#include <sys/time.h>
#include <time.h>
#include <boost/lexical_cast.hpp>
#include <hiredis/hiredis.h>

#include <iomanip>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <memory>
#include <string>

#include "utils/base_storage.h"

// @Author Dongyue.Zhang
// @Mailto zhangdy1986(at)gmail.com
// @Date   2016-09-24

namespace utils {

class RedisStoragePolicy {
 public:
  using ReturnType = std::unique_ptr<redisReply, void(*)(void*)>;
  using HandlerType = std::vector<std::string>;

  RedisStoragePolicy():
    _host("127.0.0.1"),
    _port(6378),
    _password("") {}

  ~RedisStoragePolicy() {
    Close();
  }

  ReturnType Get(HandlerType& cmd);

  bool Init(std::string const& host,
      int port,
      std::string const& passwd = "");

  int Connect();
  int Close();

 private:
  std::string _host;
  int _port;
  std::string _password;
  thread_local static redisContext* _redis_ctx;
};

}  // namespace utils

#endif  // SRC_UTILS_REDIS_STORAGE_POLICY_H_

/* vim :set ts=2 sts=2 sw=2 tw=80 et */

