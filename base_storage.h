//====================================================
// Copyright (c) 2016-2019 ZIPPY.ZDY All Rights Reserved.
//====================================================
#ifndef SRC_UTILS_BASE_STORAGE_H_
#define SRC_UTILS_BASE_STORAGE_H_

#include <memory>
#include <iomanip>
#include <stdexcept>
#include <utility>

// @Author zhangdy1986(at)gmail.com
// @Date 2016-09-25

namespace utils {

template <typename StoragePolicy>
class Storage {
 public:
  Storage():_p_storage_policy(nullptr) {}

  template <typename ... Params>
  bool Init(Params... args);

  ~Storage() = default;

  typename StoragePolicy::ReturnType
  Get(typename StoragePolicy::HandlerType & handler);

  typename StoragePolicy::ReturnType
  GetWithRetry(typename StoragePolicy::HandlerType& handler, int retry = 2);

 private:
  std::shared_ptr<StoragePolicy> _p_storage_policy;
};  // Class Storage

template <typename StoragePolicy>
template <typename... Params>
bool Storage<StoragePolicy>::Init(Params... parameters) {
  _p_storage_policy = std::make_shared<StoragePolicy>();
  if (!_p_storage_policy) {
    throw std::runtime_error("STORAGE:Init StoragePolicy Failed.");
    return false;
  }

  return _p_storage_policy->Init(parameters...);
}  // Init

template <typename StoragePolicy>
typename StoragePolicy::ReturnType
Storage<StoragePolicy>::Get(typename StoragePolicy::HandlerType & handler) {
  try {
    _p_storage_policy->Connect();
    return std::move(_p_storage_policy->Get(handler));
  } catch (...) {
    _p_storage_policy->Close();
    throw;
  }
}  // Get

template <typename StoragePolicy>
typename StoragePolicy::ReturnType
Storage<StoragePolicy>::GetWithRetry(
    typename StoragePolicy::HandlerType& handler, int retry) {
  for (int i = 1; i <= retry; ++i) {
    try {
      return std::move(Get(handler));
    } catch (std::exception const &) {
      if (i == retry)
        throw;
    }
  }
}  // GetWithRetry

}  // namespace utils

#endif  // SRC_UTILS_BASE_STORAGE_H_

/* vim :set ts=2 sts=2 sw=2 tw=80 et */
