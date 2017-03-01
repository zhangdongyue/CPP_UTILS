// Copyright (c) 2016-2017 ZIPPY.ZHANG
// mailto:zhangdy1986@gmail.com

#ifndef SRC_MOBILE_UTILS_STRING_UTILS_H_
#define SRC_MOBILE_UTILS_STRING_UTILS_H_

#include <cstring>
#include <string>

namespace utils {

/*
template <typename F>
bool Split(std::string sentence, const char* delim, F function) {
  if (sentence.empty())
    return true;
  const char * s = sentence.c_str();
  const char * tokenPtr = strtok(const_cast<char*>(s), delim);
  if (tokenPtr == NULL)
    return true;

  while (tokenPtr != NULL) {
    function(tokenPtr);
    tokenPtr = strtok(NULL, delim);
  }
  return true;
} */

template <typename F>
bool StrSplit(std::string sentence,
              char delim,
              F function) {
  if (sentence.empty())
    return true;

  const char * ss = sentence.c_str();
  const char * pos = ss, *next = ss;

  while (true) {
    pos = strchr(next, delim);

    if (!pos) {
      function(next, sentence.length() - (next - ss));
      break;
    }

    if (pos > next)
      function(next, pos - next);

    next = pos + 1;

    if ((next - ss) >= sentence.length())
      break;
  }

  return true;
}

template <typename F>
bool MemSplit(const char * sentence,
              const char delim,
              size_t count,  F function) {
  if (!sentence || count == 0)
    return true;

  const char * pos = sentence, *next = sentence;

  while (true) {
    pos = reinterpret_cast<const char*>(
        memchr(reinterpret_cast<const void*>(next), delim, count));

    if (!pos) {
      function(next, count);
      break;
    }

    size_t token_len = pos - next;
    if (token_len > 0) {
      function(next, token_len);
    }

    count -= token_len + 1;
    if (count <= 0)
      break;
    next = pos + 1;
  }

  return true;
}

bool DualSplit(const char* sentence, 
               const char delim, 
               size_t count,
               std::string& first,
               std::string& second);

}  // namespace utils

#endif  //  SRC_MOBILE_UTILS_STRING_UTILS_H_

/* vim: set ts=2 sts=2 sw=2 tw=80 et */
