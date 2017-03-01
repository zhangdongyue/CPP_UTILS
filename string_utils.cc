// Copyright (c) 2016-2017 ZIPPY.ZHANG
// mailto:zhangdy1986@gmail.com

#include "mobile/utils/string_utils.h"
#include <cstring>
#include <string>

namespace utils {

bool DualSplit(const char* sentence,
               const char delim,
               size_t count,
               std::string& first,
               std::string& second) {
  if (!sentence || count == 0)
    return true;

  const char* pos = NULL;
  pos = reinterpret_cast<const char*>(
      memchr(reinterpret_cast<const void*>(sentence), delim, count));
  if (!pos) {
    return false;
  }

  size_t first_len = pos - sentence;
  size_t second_len = count - first_len - 1;

  first.assign(sentence, first_len);
  second.assign(sentence + first_len + 1, second_len);

  return true;
}

}  // namespace utils

/* vim: set ts=2 sts=2 sw=2 tw=80 et */
