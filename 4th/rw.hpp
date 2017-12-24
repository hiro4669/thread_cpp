/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   rw.hpp
 * Author: hiroaki
 *
 * Created on November 21, 2017, 9:14 AM
 */

#ifndef _RW_HPP_
#define _RW_HPP_

#include <string>
#include "Thread.hpp"



class RWLock {
private:
    int readCount;
    pthread_mutex_t mutex;
    pthread_cond_t  mcond;
    bool lock;

public:
    RWLock();
    ~RWLock();
    void readLock(int id);
    void readUnlock(int id);
    void writeLock(int id);
    void writeUnlock(int id);
    void invoke(int id); // dummy
};

class Data {
private:
    int num;
    RWLock *lock;
    void slowly();
    /* for test */
    int doReadTest();
    int doWriteTest();
public:
    Data();
    Data(RWLock *lock);
    ~Data();
    /* for test */
    int read(int id);
    int unlockRead(int id);
    int write(int id);
    int unlockWrite(int id);
};


class ReadThread : public Thread {
private:
    Data *pData;
    int id;
    //RWLock *lock;
    void lock_unlock_test();

public:
    ReadThread();
    ReadThread(Data *data);
    ~ReadThread();
    void* run(void *arg);
    void showbuf(char* buf, int size);
};

class WriteThread : public Thread {
private:
    Data *pData;
    int id;
    std::string filter;
public:
    WriteThread();
    WriteThread(Data *data);
    ~WriteThread();
    void* run(void *arg);
};

#endif /* RW_HPP */
