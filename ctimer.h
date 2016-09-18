//================================================
//  Copyright (c) 2016 ZIPPY.Z All Rights Reserved.
//================================================

#ifndef SRC_UTILS_CTIMER_H_
#define SRC_UTILS_CTIMER_H_
#include<pthread.h>
#include<stdint.h>
#include<sys/time.h>
#include<sys/select.h>

/*
 * @Author Dongyue.Zhang 
 * @Mail  zhangdy1986(at)gmail.com
 * */

namespace utils {

class CTimer {
 private:
    pthread_t thread_timer;
    int64_t m_second, m_microsecond;

    static void *OnTimer_stub(void *p);

    void thread_proc();

    /* extend this class and write your OnTimer() */
    virtual void OnTimer() noexcept = 0;
 public:
    CTimer();
    CTimer(int64_t m_second, int64_t m_microsecond);
    virtual ~CTimer();
    void SetTimer(int64_t second, int64_t microsecond);
    void StartTimer();
    void StopTimer();
};

}  // namespace utils

#endif  // SRC_UTILS_CTIMER_H_

/* vim: set ts=2 sts=2 sw=2 tw=80 et */
