#ifndef _RW_HPP_
#define _RW_HPP_

#include <string>
#include "Thread.hpp"


class ReadThread : public Thread {
private:
    int id;

public:

    ReadThread();
    ~ReadThread();
    void* run(void *arg);
};

#endif /* RW_HPP */
