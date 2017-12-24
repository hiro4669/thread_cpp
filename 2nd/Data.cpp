#include <stdio.h>
#include <unistd.h>

#include "rw.hpp"

Data::Data() {
    this->buffer = new char[this->bufSize = 10];
    printf("data constructor\n");
}

Data::Data(int size) {
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
    return this->doRead();
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
    this->doWrite(c);
}

void Data::doWrite(char c) {
    for (int i = 0; i < this->bufSize; ++i) {
        this->buffer[i] = c;
        this->slowly();
    }
}

void Data::slowly() {
//    sleep(1);
//    usleep(1000000); //1sec

    usleep(500000); //1sec
}

int Data::getBufSize() {
    return this->bufSize;
}
