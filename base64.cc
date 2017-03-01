#include "base64.h"
#include <iostream>

/*
 *  @author bjzhangdongyue
 *  @date   2017/02/28
 *  @brief  base64 encode
 * */

namespace utils {

static void encode_base64_inernal(std::string& dst, 
    const char* src, 
    size_t src_len,
    const char* basis,
    bool padding) {

  const char *s = src;

  size_t len = src_len;
  dst.reserve((len/2 + 1)*3);

  while (len > 2) {
    dst.push_back(basis[(s[0] >> 2) & 0x3f]);
    dst.push_back(basis[((s[0] & 3) << 4) | (s[1] >> 4)]);
    dst.push_back(basis[((s[1] & 0x0f) << 2) | (s[2] >> 6)]);
    dst.push_back(basis[s[2] & 0x3f]);

    s += 3;
    len -= 3;
  }

  if (len) {
    dst.push_back(basis[(s[0] >> 2) & 0x3f]);

    if (len == 1) {
      dst.push_back(basis[(s[0] & 3) << 4]);
      if (padding) {
        dst.push_back('=');
      }
    } else {
      dst.push_back(basis[((s[0] & 3) << 4) | (s[1] >> 4)]);
      dst.push_back(basis[(s[1] & 0x0f) << 2]);
    }

    if (padding) {
      dst.push_back('=');
    }
  }

  return;
}

void encode_base64(std::string& dst, 
    const char* src, 
    size_t src_len) {
  static char basis64[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  encode_base64_inernal(dst, src, src_len, basis64, true);

}

void encode_base64_url(std::string& dst,
    const char* src,
    size_t src_len) {
  static char basis64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
  encode_base64_inernal(dst, src, src_len, basis64, false);
}

}  // namespace utils
