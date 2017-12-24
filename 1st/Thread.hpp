#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#include <pthread.h>
#include <cstdlib>

class Thread {
private:
    pthread_t thread;
    void *args;

protected:
    static int tcount;

public:
    Thread(){tcount++; /*printf("thread\n");*/}
    virtual ~Thread() {}
    virtual void* run(void *arg) = 0;

    static void* wrap(void *arg) {
        Thread* pt = (Thread*)arg;
        return pt->run(pt->args);
    }
    void start(void *arg) {
        this->args = arg;
        if (pthread_create(&this->thread, NULL, wrap, this)) {
            printf("thread create error\n");
            abort();
        }
    }

    void wait() {
        if (pthread_join(this->thread, NULL)) {
            printf("join error");
            abort();
        }
    }
};

//#include "static.hpp"
//int Thread::tcount = -1;

#endif /* THREAD_HPP */
