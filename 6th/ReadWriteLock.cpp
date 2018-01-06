
#include "rw.hpp"

ReadWriteLock::ReadWriteLock() {
    this->readingReaders = 0;
    this->waitingWriters = 0;
    this->writingWriters = 0;
    this->preferWriter = true;
}

ReadWriteLock::~ReadWriteLock() {        
}

void ReadWriteLock::readLock() {
    {    
        std::unique_lock<std::mutex> uniq_lk(mtx); // lock
        while (writingWriters > 0 || (preferWriter && waitingWriters > 0)) {
            cv.wait(uniq_lk);
        }
        this->readingReaders++;        
    }
}

void ReadWriteLock::readUnlock() {
    {
        std::unique_lock<std::mutex> uniq_lk(mtx); // lock
        this->readingReaders--;
        this->preferWriter = true;
        cv.notify_all();
    }
}

void ReadWriteLock::writeLock() {
    {
        std::unique_lock<std::mutex> uniq_lk(mtx); // lock
        this->waitingWriters++;
        while (readingReaders > 0 || writingWriters > 0) {
            cv.wait(uniq_lk);
        }
        this->waitingWriters--;
        this->writingWriters++;
    }
}

void ReadWriteLock::writeUnlock() {
    {
        std::unique_lock<std::mutex> uniq_lk(mtx); // lock
        this->writingWriters--;
        this->preferWriter = false;
        cv.notify_all();        
    }
}





