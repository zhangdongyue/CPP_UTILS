// Copyright (c) 2016-2017 DONGYUE.ZHANG
// mail to zhangdy1986(at)gmail.com

#ifndef SRC_UTILS_LRU_CACHE_H_
#define SRC_UTILS_LRU_CACHE_H_

#include <unordered_map>
#include <utility>
#include <list>
#include <string>
#include <thread>
#include <ostream>

// @Author bjzhangdongyue
// @Date   2017-03-27 11:30:00
// @Brief  LRU cache
// @Note   not threadsafe, need to do mutual exclusion when Get or Set

using std::list;
using std::unordered_map;
using std::pair;
using std::string;

namespace utils {

template <typename TKEY, typename TVALUE>
class LRUCache {
 public:
  explicit LRUCache(int capacity) : capacity_(capacity) {}
  
  /* @params[in] function : 
   *            Implement it when need a MISSING judgement,
   *            such as Expire or others. Here its should be 
   *            a method such as : bool func(TVALUE val).
   *            Default [nullptr]
   * */
  template <typename F>
  TVALUE* Get(TKEY key, F function = nullptr);
  
  void Set(TKEY key, TVALUE value);

  struct RETURN {
    enum type{
      NONE = 0,
      HIT = 1,
      MISS = 2
    };
  };

  static void SetErrMsg(int err_code, const std::string& err_str) {
    err_msg_ = {err_code, err_str};
  }

  inline const pair<int, string>& GetErrMsg() const {
    return err_msg_;
  }

 private:
  LRUCache(const LRUCache<TKEY, TVALUE>&) = delete;
  LRUCache(const LRUCache<TKEY, TVALUE>&&) = delete;
  LRUCache<TKEY, TVALUE>& operator=(const LRUCache<TKEY, TVALUE>&) = delete;

 private:
  using L = list<TKEY>;
  using PL =  pair<TVALUE, typename L::iterator>;
  using HPL =  unordered_map<TKEY, PL>;

  /* @Breif If HIT, only adjust the KEY(list used_) */
  void Touch(typename HPL::iterator it);

  void Delete(typename HPL::iterator it);

  HPL cache_;
  L used_;
  int capacity_;

  static thread_local pair<int ,string> err_msg_;
};  // Class LRUCache

std::ostream& operator<<(std::ostream &out, const pair<int, string>& err_msg) {
  out << err_msg.first << " : " << err_msg.second;
  return out;
} 

template <typename TKEY, typename TVALUE>
thread_local pair<int ,string> LRUCache<TKEY, TVALUE>::err_msg_ = 
                          {LRUCache<TKEY, TVALUE>::RETURN::NONE, "none"};

template <typename TKEY, typename TVALUE>
template <typename F>
TVALUE* LRUCache<TKEY, TVALUE>::Get(TKEY key, F function) {
  auto it = cache_.find(key);

  if (it == cache_.end()) {
    err_msg_ = {RETURN::MISS, "CACHE MISSING"}; 
    return nullptr;
  }

  if (nullptr != function) {
    /* You should set err_msg_ in function,
     * when it return false.
     * or it will be set default */
    if (function(it->second.first)) {
      Delete(it);
      return nullptr;
    }
  }

  Touch(it);
  err_msg_ = {RETURN::HIT, "CACHE HIT"}; 

  return &(it->second.first);
}  // Get

template <typename TKEY, typename TVALUE>
void LRUCache<TKEY, TVALUE>::Set(TKEY key, TVALUE value) {
  auto it = cache_.find(key);

  if (it != cache_.end()) {
    Touch(it);
    return;
  } else {
    if (cache_.size() == capacity_) {
      cache_.erase(used_.back());
      used_.pop_back();
    }
    used_.push_front(key);
  }

  cache_[key] = { value, used_.begin() };
}  // Set

template <typename TKEY, typename TVALUE>
void LRUCache<TKEY, TVALUE>::Touch(typename HPL::iterator it) {
  int key = it->first;
  used_.erase(it->second.second);
  used_.push_front(key);
  it->second.second = used_.begin();
}  // Touch

template <typename TKEY, typename TVALUE>
void LRUCache<TKEY, TVALUE>::Delete(typename HPL::iterator it) {
  used_.erase(it->second.second);
  cache_.erase(it);
}  // Delete

}  // namespace utils

#endif  // SRC_UTILS_LRU_CACHE_H_

/* vim :set ts=2 sts=2 sw=2 tw=80 et */
