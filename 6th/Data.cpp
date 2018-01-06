#include <thread>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include "rw.hpp"

Data::Data(int size, ReadWriteLock& l) : lock(l), num(0) {
    srand(100);
    this->buffer = new char[this->bufSize = size];
    for (int i = 0; i < this->bufSize; ++i) {
        this->buffer[i] = '*';
    }
}
Data::~Data() {
    delete[] this->buffer;
}

char* Data::read() {
    lock.readLock();
    char* data = doRead();
    lock.readUnlock();
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
    lock.writeLock();
    this->doWrite(c);
    lock.writeUnlock();
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