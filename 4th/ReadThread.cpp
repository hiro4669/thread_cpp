#include <stdio.h>
#include <unistd.h>
#include "rw.hpp"

ReadThread::ReadThread() : pData(NULL) {
}

ReadThread::ReadThread(Data* data) {
    this->id = tcount;
    this->pData = data;
}
ReadThread::~ReadThread() {

}

void* ReadThread::run(void* arg) {
    printf("run Thread %d\n", this->id);
//    int num = this->pData->unlockRead(this->id);
    int num = this->pData->read(this->id);
    printf("Thread%d num = %d\n", this->id, num);
    return NULL;
}
