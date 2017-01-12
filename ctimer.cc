//================================================
//  Copyright (c) 2016 ZIPPY.Z All Rights Reserved.
//================================================

#include "ctimer.h"
#include <errno.h>

/*
 * @Author Donyue.Zhang 
 * @Mail zhangdy1986(at)gmail.com
 * */

namespace utils {

CTimer::CTimer():m_second(0), m_microsecond(0) {}

CTimer::CTimer(int64_t second, int64_t microsecond):
  m_second(second),
  m_microsecond(microsecond) {}

CTimer::~CTimer() {}

void* CTimer::OnTimer_stub(void *p) {
  (static_cast<CTimer*>(p))->thread_proc();
}

void CTimer::SetTimer(int64_t second, int64_t microsecond) {
  m_second = second;
  m_microsecond = microsecond;
}

void CTimer::StartTimer() {
  pthread_create(&thread_timer, NULL, OnTimer_stub, this);
}

void CTimer::StopTimer() {
  pthread_cancel(thread_timer);
  pthread_join(thread_timer, NULL);
}

void CTimer::thread_proc() {
  while (true) {
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    OnTimer();
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_testcancel();
    struct timeval tempval;
    tempval.tv_sec = m_second;
    tempval.tv_usec = m_microsecond;
    int err = 0;
    do {
      select(0, NULL, NULL, NULL, &tempval);
    } while (err < 0 && errno == EINTR);
  }
}

}  // namespace utils

/* vim: set ts=2 sts=2 sw=2 tw=80 et */
