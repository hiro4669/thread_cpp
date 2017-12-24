
#ifndef _RW_HPP_
#define _RW_HPP_

#include <string>
#include "Thread.hpp"


class ReadWriteLock {
private:
    pthread_mutex_t mutex;
    pthread_cond_t mcond;
    int readingReaders;
    int waitingWriters;
    int writingWriters;
    bool preferWriter;

public:
    ReadWriteLock();
    ~ReadWriteLock();
    void readLock();
    void readUnlock();
    void writeLock();
    void writeUnlock();
};

class Data {
private:
    int num;
    int bufSize;
    char *buffer;
    ReadWriteLock *lock;

    void slowly();
    char* doRead();
    void doWrite(char c);

public:
    Data();
    Data(int size, ReadWriteLock *lock);
    ~Data();

    char* read();
    void write(char c);
    int getBufSize();
};


class ReadThread : public Thread {
private:
    Data *pData;
    int id;
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
    const char* filter;
    int fil_len;
    int index;

    char nextChar();

public:
    WriteThread();
    WriteThread(Data *data, const char *s);
    ~WriteThread();
    void* run(void *arg);
};

#endif /* RW_HPP */
