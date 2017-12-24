#include <stdio.h>
#include <unistd.h>

#include "rw.hpp"

Data::Data() : lock(NULL){
    this->num = 0;
    this->buffer = new char[this->bufSize = 10];
    srand(100);
    printf("data constructor\n");
}

Data::Data(int size, ReadWriteLock *lock) {
    srand(100);
    this->num = 0;
    this->lock = lock;
    this->buffer = new char[this->bufSize = size];
    printf("data constructor with size\n");
    for (int i = 0; i < this->bufSize; ++i) {
        this->buffer[i] = '*';
    }
}
Data::~Data() {
    printf("data denstructor\n");
    delete[] this->buffer;
}

char* Data::read() {
    lock->readLock();
    char* data = doRead();
    lock->readUnlock();
    return data;
}

char* Data::doRead() {
    char *newbuf = new char[this->bufSize];
    for (int i = 0; i < this->bufSize; ++i) {
        newbuf[i] = this->buffer[i];
    }
    this->slowly();
    return newbuf;
}

void Data::write(char c) {
    lock->writeLock();
    this->doWrite(c);
    lock->writeUnlock();
}

void Data::doWrite(char c) {
    for (int i = 0; i < this->bufSize; ++i) {
        this->buffer[i] = c;
        this->slowly();
    }
}

void Data::slowly() {
    usleep(50000); 
}

int Data::getBufSize() {
    return this->bufSize;
}
