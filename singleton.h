//=====================================================
// Copyright (c) 2015-2016 ZIPPY.Z All Rights Reserved.
//=====================================================

#ifndef SRC_UTILS_SINGLETON_H_
#define SRC_UTILS_SINGLETON_H_

#include <pthread.h>

// @Author dongyue.zhang 
// @Mailto zhangdy1986(at)gmail.com

namespace utils {

template<typename T>
class Singleton {
 public:
    inline static T& getInstance() {
      pthread_once(&_once_control, init);
      return *_value;
    }

 private:
    inline static void init() {
      _value = new(std::nothrow) T();
    }

    Singleton();
    ~Singleton();

    static pthread_once_t _once_control;
    static T *_value;
};

template<typename T>
pthread_once_t Singleton<T>::_once_control = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::_value = NULL;

}  // namespace utils

#endif  // SRC_UTILS_SINGLETON_H_

/* vim: set ts=2 sts=2 sw=2 tw=80 et */
