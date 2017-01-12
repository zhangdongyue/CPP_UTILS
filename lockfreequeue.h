//====================================================
// Copyright (c) Dongyue.Zippy (zhangdy1986@gmail.com)
//====================================================

#ifndef LOCKFREEQUEUE_H_ 
#define LOCKFREEQUEUE_H_ 
#include <atomic>
#include <memory>
#include <ctime>
#include <cerrno>
#include <type_traits>

extern "C" {
#include <pthread.h>
#include <semaphore.h>
}

/*
 * @Author zhangdongyue
 * @Brief ThreadSafe Queue by C++11
 * */

#define barrier() __asm__ __volatile__("" ::: "memory")
#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&x)

namespace utils {

class Semaphore {
 public:
   Semaphore(int initial_count = 0) {
     assert(initialCount >= 0);
     sem_init(&m_sema, 0, initial_count);
   }

   ~Semaphore() {
     sem_destroy(&m_sema);
   }

   void wait() {
     int rc;
     do {
       rc = sem_wait(&m_sema);
     } while (rc == -1 && errno == EINTR);
   }

   bool try_wait() {
     int rc;
     do {
       rc = sem_trywait(&m_sema);
     } while (rc == -1 && errno == EINTR);

     return !(rc == -1 && errno == EAGAIN);
   }

   bool timed_wait(std::uint64_t usecs) {
     struct timespec ts;
     const int usecs_in_1_sec = 1000000;
     const int nsecs_in_1_sec = 1000000000;
     clock_gettime(CLOCK_REALTIME, &ts);
     ts.tv_sec += usecs / usecs_in_1_sec;
     ts.tv_nsec += (usecs % usecs_in_1_sec) * 1000;

     if (ts.tv_nsec > nsecs_in_1_sec) {
       ts.tv_nsec -= nsecs_in_1_sec;
       ++ts.tv_sec;
     }

     int rc;
     do {
       rc = sem_timedwait(&m_sema, &ts);
     } while (rc == -1 && errno == EINTR);

     return !(rc == -1 && errno == ETIMEDOUT);
   }

   void signal() {
     sem_post(&m_sema);
   }

   void single(int count) {
     while (count-- > 0) {
       sem_post(&m_sema);
     }
   }

 private:
   sem_t m_sema;
   Semaphore(const Semaphore& other) = delete;
   Semaphore& operator=(const Semaphore& other) = delete;

};  // Class Semaphore

class LightWightSemaphore {
 public:
   typedef std::make_signed<std::size_t>::type ssize_t;

 public:
   LightWightSemaphore(ssize_t initial_count = 0) : 
     m_count(initial_count) {
     assert(initial_count > 0);  
   }

   bool tryWait() {
     ssize_t oldCount = m_count.load(std::memory_order_relaxed);
     while (oldCount > 0) {
       if (m_count.compare_exchange_weak(oldCount,
             oldCount - 1,
             std::memory_order_acquire,
             std::memory_order_relaxed))
         return true;
     }
     return false;
   }

   void wait() {
     if (!tryWait())
       waitWithPartialSpinning();
   }

   ssize_t tryWaitMany(ssize_t max) {
     assert(max >= 0);
     ssize_t oldCount = m_count.load(std::memory_order_relaxed);
     while (oldCount > 0) {
       ssize_t newCount = oldCount > max ? oldCount - max : 0;
       if (m_count.compare_exchange_weak(oldCount, 
             newCount, 
             std::memory_order_acquire, 
             std::memory_order_relaxed))
         return oldCount - newCount;
     }
     return 0;
   }

   ssize_t waitMany(ssize_t max, std::int64_t timeout_usecs) {
     assert(max >= 0);
     ssize_t result = tryWaitMany(max);
     if (result == 0 && max > 0)
       result = waitManyWithPartialSpinning(max, timeout_usecs);
     return result;
   }
   
   ssize_t waitMany(ssize_t max) {
     ssize_t result = waitMany(max, -1);
     assert(result > 0);
     return result;
   }

   void signal(ssize_t count = 1) {
     assert(count >= 0);
     ssize_t oldCount = m_count.fetch_add(count, std::memory_order_release);
     ssize_t toRelease = -oldCount < count ? -oldCount : count;
     if (toRelease > 0) {
       m_sema.signal((int)toRelease);
     }
   }
   
   ssize_t availableApprox() const {
     ssize_t count = m_count.load(std::memory_order_relaxed);
     return count > 0 ? count : 0;
   }

 private:
   std::atomic<ssize_t> m_count;
   Semaphore m_sema;

   bool waitWithPartialSpinning(std::int64_t timeout_usecs = -1) {
     ssize_t oldCount;
     int spin = 10000;
     while (--spin >= 0) {
       oldCount = m_count.load(std::memory_order_relaxed);
       if ((oldCount > 0) && m_count.compare_exchange_strong(oldCount, 
             oldCount - 1, 
             std::memory_order_acquire,
             std::memory_order_relaxed))
         return true;
       std::atomic_signal_fence(std::memory_order_acquire);
     }
     oldCount = m_count.fetch_sub(1, std::memory_order_acquire);
     if (oldCount > 0)
       return true;

     if (timeout_usecs < 0) {
       m_sema.wait();
       return true;
     }

     if (m_sema.timed_wait((std::uint64_t)timeout_usecs))
       return true;

     while (true) {
       oldCount = m_count.load(std::memory_order_acquire);
       if (oldCount >= 0 && m_sema.try_wait())
         return true;
       if (oldCount < 0 && m_count.compare_exchange_strong(oldCount,
             oldCount + 1,
             std::memory_order_relaxed))
         return false;
     }
   }

   ssize_t waitManyWithPartialSpinning(ssize_t max, std::int64_t timeout_usecs = -1) {
     assert(max > 0);
     ssize_t oldCount;
     int spin = 10000;
     while (--spin >= 0) {
       oldCount = m_count.load(std::memory_order_relaxed);
       if (oldCount > 0) {
         ssize_t newCount = oldCount > max ? oldCount - max : 0;
         if (m_count.compare_exchange_strong(oldCount, 
               newCount, 
               std::memory_order_acquire, 
               std::memory_order_relaxed))
           return oldCount - newCount;
       }
       std::atomic_signal_fence(std::memory_order_acquire);
     }
     oldCount = m_count.fetch_sub(1, std::memory_order_acquire);
     if (oldCount <= 0) {
       if (timeout_usecs < 0)
         m_sema.wait();
       else if (!m_sema.timed_wait((std::uint64_t)timeout_usecs)) {
         while (true) {
           oldCount = m_count.load(std::memory_order_acquire);
           if (oldCount >= 0 && m_sema.try_wait())
             break;
           if (oldCount < 0 && m_count.compare_exchange_strong(oldCount, 
                 oldCount + 1, 
                 std::memory_order_relaxed))
             return 0;
         }
       }
     }
     if (max > 1)
       return 1 + tryWaitMany(max - 1);
     return 1;
   }
};  // Class LightWightSemaphore

template <typename T>
class LockFreeQueue {
  typedef size_type std::uint32_t;
  public:
    LockFreeQueue() : i_cursor(0),i_size(0) {}
    explicit LockFreeQueue(size_type size) : 
      i_cursor(0),
      i_size(size)
    virtual ~LockFreeQueue() = default;

    void pop();
    void push(const T& val);

    T& front();
    const T& front() const;

    T& back();
    const T& back() const;

    size_type size() const {
      return i_size;
    }

    template <class... Args>
    void emplace(Args&&... args);

    void swap(LockFreeQueue& x) noexcept;

    bool empty() const {
      return (!size());
    }

  protected:
    std::atomic<size_type> i_cursor;
    volatile size_type i_size;

  private:
    LockFreeQueue(const LockFreeQueue& other) = delete;
    LockFreeQueue& operator=(const LockFreeQueue& other) = delete;
}

template <typename T>
void LockFreeQueue<T>::pop() {
  --i_cursor;
}

template <typename T>
void LockFreeQueue<T>::push(const T& val) {
  do {
    ++i_cursor;
    size_type idx = i_cursor;
  } while (!atomic_compare_exchange_strong(i_cursor, idx, idx))

  // TODO
  container[i_cursor] = val;
}

}  // namespace utils

#endif  // LOCKFREEQUEUE_H_ 

/* vim: set ts=2 sw=2 sts=2 tw=88 et */

