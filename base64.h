#include <string> 

/*
 *  @author bjzhangdongyue
 *  @date   2017/02/28
 *  @brief  base64 encode
 * */

namespace utils {

void encode_base64(std::string& dst, const char* src, size_t src_len);

void encode_base64_url(std::string& dst, const char* src, size_t src_len);

}  // namespace utils
