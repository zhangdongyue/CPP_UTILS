//=====================================================
// Copyright(c) 2015-2017 ZIPPY.ZDY All Rights Reserved.
//=====================================================
#include "utils/web_storage_policy.h"

// @Author DONGYUE.ZHANG 
// @Mailto zhangdy1986(at)gmail.com
// @Date   2016-09-24

#include <errno.h>
#include <utility>
#include <string>

namespace utils {

#define THROW_EXCEPTION(str) { \
  std::string err_str = "WEBSTORAGE ERROR:"; \
  err_str.append(str); \
  err_str.append(strerror(errno)); \
  throw std::runtime_error(std::move(err_str)); \
}

thread_local CURL* WebStoragePolicy::_curl_ctx = nullptr;

size_t WebStoragePolicy::WriteToString(char * ptr, size_t size,
    size_t nmemb, void* userdata) noexcept {
  auto ptr_size = size * nmemb;
  if (ptr_size == 0)
    return 0;

  try {
    std::string *str = static_cast<std::string*>(userdata);
    str->append(ptr, ptr_size);
  } catch (...) {
    return 0;
  }

  return ptr_size;
}  // WriteToString

int WebStoragePolicy::Connect() {
  if (_curl_ctx)
    return 0;

  if (CURLE_OK != curl_global_init(CURL_GLOBAL_NOTHING)) {
    THROW_EXCEPTION("curl_global_init : ");
  }

  _curl_ctx = curl_easy_init();
  if (NULL == _curl_ctx) {
    THROW_EXCEPTION("curl_easy_init : ");
  }

  curl_easy_setopt(_curl_ctx, CURLOPT_VERBOSE, 0);
  curl_easy_setopt(_curl_ctx, CURLOPT_CONNECTTIMEOUT, 3);
  curl_easy_setopt(_curl_ctx, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(_curl_ctx, CURLOPT_MAXREDIRS, 5);

  return 0;
}  // Connect

WebStoragePolicy::ReturnType WebStoragePolicy::Get(
    HandlerType& url) {
  CURLcode curl_rc = curl_easy_setopt(_curl_ctx, CURLOPT_URL, url.c_str());
  if (curl_rc != CURLE_OK) {
    THROW_EXCEPTION("curl_easy_setopt : ");
  }

  ReturnType response;

  curl_easy_setopt(_curl_ctx, CURLOPT_WRITEFUNCTION, WriteToString);
  curl_easy_setopt(_curl_ctx, CURLOPT_WRITEDATA, &response);

  curl_rc = curl_easy_perform(_curl_ctx);

  if (CURLE_OK != curl_rc) {
    THROW_EXCEPTION("curl_easy_perform : ")
  }

  return std::move(response);
}  // Get


}  // namespace utils

/* vim :set ts=2 sts=2 sw=2 tw=80 et */
