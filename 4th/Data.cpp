#include <stdio.h>
#include <unistd.h>

#include "rw.hpp"

Data::Data() : lock(NULL){
    this->num = 0;
    srand(100);
    printf("data constructor\n");
}

Data::Data(RWLock *lock) {
    srand(100);
    this->num = 0;
    this->lock = lock;
    printf("data constructor with size\n");
}
Data::~Data() {
    printf("data denstructor\n");
}

int Data::read(int id) {
    lock->readLock(id);
    int v = doReadTest();
    lock->readUnlock(id);
    return v;
}

int Data::unlockRead(int id) {
    return doReadTest();
}

int Data::write(int id) {
    lock->writeLock(id);
    int v = doWriteTest();
    lock->writeUnlock(id);
    return v;
}

int Data::unlockWrite(int id) {
    return doWriteTest();
}

int Data::doReadTest() {

    for (int i = 0; i < 100; ++i) {
        --num;
        slowly();
    }
    return this->num;
}

int Data::doWriteTest() {
    for (int i = 0; i < 300; ++i) {
        ++num;
        slowly();
    }
    return num;
}

void Data::slowly() {
    int rest = (rand() % 50) * 100;
    usleep(rest); //1sec
}
