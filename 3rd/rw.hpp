#ifndef _RW_HPP_
#define _RW_HPP_

#include <string>
#include "Thread.hpp"

class RWLock {
private:
    pthread_mutex_t mutex;
    pthread_cond_t  mcond;
    bool lock;

public:
    RWLock();
    ~RWLock();
    void readLock(int id);
    void readUnlock(int id);
};



class ReadThread : public Thread {
private:
    int id;
    RWLock *lock;

    void lock_unlock_test();
public:

    ReadThread();
    ReadThread(RWLock *lock);
    ~ReadThread();
    void* run(void *arg);
};
#endif /* RW_HPP */
