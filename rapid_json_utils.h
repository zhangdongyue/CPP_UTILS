/*=====================================================
 *Copyright (c) 2015-2016 ZIPPY.ZDY all Rights Reserved.
 *=====================================================*/
#ifndef SRC_UTILS_RAPID_JSON_UTILS_H_
#define SRC_UTILS_RAPID_JSON_UTILS_H_

#include <rapidjson/rapidjson.h>
#include <string>

// @Author Dongyue.Zhang,zhangdy1986(at)gmail.com
// @Date 2016-09-13 15:00:00
// @Breif rapidjson utils functions

namespace utils {

class RapidJsonUtils {
 public:
  template <typename ObjectType>
  static bool GetString(
      std::string &val,
      const ObjectType& obj,
      const char* field);

  template <typename ObjectType, typename NumberType>
  static bool GetNumber(NumberType& val,
      const ObjectType& obj,
      const char* fleld);
};

template <typename ObjectType>
bool RapidJsonUtils::GetString(std::string& val,
    const ObjectType& obj,
    const char* field) {
  const auto &iter = obj.FindMember(field);
  if (iter == obj.MemberEnd() || !iter->value.IsString())
    return false;

  val.assign(iter->value.GetString(), iter->value.GetStringLength());
  return true;
}

template <typename ObjectType, typename NumberType>
bool RapidJsonUtils::GetNumber(NumberType& val,
    const ObjectType& obj,
    const char* field) {
  const auto& iter = obj.FindMember(field);
  if (iter == obj.MemberEnd())
    return false;

  if (iter->value.IsInt()) {
    val = iter->value.GetInt();
    return true;
  } else if (iter->value.IsUint()) {
    val = iter->value.GetUint();
    return true;
  } else if (iter->value.IsInt64()) {
    val = iter->value.GetInt64();
    return true;
  } else if (iter->value.IsUint64()) {
    val = iter->value.GetUint64();
    return true;
  } else if (iter->value.IsDouble()) {
    val = iter->value.GetDouble();
    return true;
  } else if (iter->value.IsBool()) {
    val = iter->value.GetBool();
    return true;
  }

  return false;
}

}  // namespace utils

#endif  // SRC_UTILS_RAPID_JSON_UTILS_H_

/* vim: set ts=2 sts=2 sw=2 tw=80 et */
