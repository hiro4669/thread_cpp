#ifndef _RW_HPP_
#define _RW_HPP_

#include <mutex>
#include <condition_variable>
#include <thread>
#include "Thread.hpp"

class RWLock {
private:
    int readCount;
    std::mutex mtx;
    std::condition_variable cv;
    bool lock;
    
public:
    RWLock();
    ~RWLock();
    void readLock(std::thread::id id);
    void readUnlock(std::thread::id id);
    void writeLock(std::thread::id id);
    void writeUnlock(std::thread::id id);
};


class ReadWriteLock {
private:
    std::mutex mtx;
    std::condition_variable cv;
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
    ReadWriteLock& lock;

    void slowly();
    char* doRead();
    void doWrite(char c);

public:
    Data();
    Data(int size, ReadWriteLock& lock);
    ~Data();

    char* read();
    void write(char c);
    int getBufSize();
};

/*
class Data {
private:
    int num;
    RWLock& lock;
    
    void slowly();
    // for test
    int doReadTest();
    int doWriteTest();
    
public:
    Data() = delete;
    Data(RWLock& lock);
    ~Data();
    //
    int read(std::thread::id id);
    int unlockRead(std::thread::id id);
    int write(std::thread::id id);
    int unlockWrite(std::thread::id id);
};
*/

class ReadThread : public Thread {
private:
    Data& data;
    int id;
public:

    ReadThread() = delete;
    ReadThread(Data& data);
    ~ReadThread();
    void run(void *arg);
    void showbuf(char* buf, int size);
};

class WriteThread : public Thread {
private:
    Data& data;
    int id;
    const char* filter;
    int fil_len;
    int index;

    char nextChar();

public:
    WriteThread() = delete;
    WriteThread(Data& data, const char *s);
    ~WriteThread();
    void run(void *arg);
};



/*
class ReadThread : public Thread {
private:
    Data& data;
public:
    ReadThread() = delete;
    ReadThread(Data& d);
    ~ReadThread();
    void run(void* args);    
};

class WriteThread : public Thread {
private:
    Data& data;
    
public:
    WriteThread() = delete;
    WriteThread(Data& d);
    ~WriteThread();
    void run(void* args);
};
*/

#endif /* RW_HPP */

