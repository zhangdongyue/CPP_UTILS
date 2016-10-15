//=====================================================
// Copyright(c) 2015-2017 ZIPPY.ZDY All Rights Reserved.
//=====================================================
#ifndef SRC_UTILS_WEB_STORAGE_POLICY_H_
#define SRC_UTILS_WEB_STORAGE_POLICY_H_

#include <curl/curl.h>
#include <time.h>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <vector>
#include <stdexcept>
#include <memory>
#include <string>

#include "utils/base_storage.h"


// @Author DONGYUE.ZHANG
// @Mailto zhangdy1986(at)gmail.com
// @Date   2016-09-24

namespace utils {

class WebStoragePolicy {
 public:
  using ReturnType = std::string;
  using HandlerType = std::string;

  WebStoragePolicy():_url("") {}

  ~WebStoragePolicy() {
    Close();
  }

  ReturnType Get(HandlerType& url);

  bool Init() {
    return true;
  }

  int Connect();

  int Close() {
    if (_curl_ctx) {
      curl_easy_cleanup(_curl_ctx);
      _curl_ctx = nullptr;
    }

    return 0;
  }

 private:
  static size_t WriteToString(char * ptr, size_t size,
      size_t nmemb, void* userdata) noexcept;

 private:
  std::string _url;
  thread_local static CURL * _curl_ctx;
};

}  // namespace utils

#endif  // SRC_UTILS_WEB_STORAGE_POLICY_H_
/* vim :set ts=2 sts=2 sw=2 tw=80 et */
