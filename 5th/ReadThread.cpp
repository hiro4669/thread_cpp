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

void ReadThread::showbuf(char *buf, int size) {
    printf("ReadThread %d read ", this->id);
    for (int i = 0; i < size; ++i) {
        printf("%c", buf[i]);
    }
    printf("\n");
}

void* ReadThread::run(void* arg) {
    printf("run Thread %d\n", this->id);
    while(true) {
        char* buf = this->pData->read();
        this->showbuf(buf, this->pData->getBufSize());
        free(buf);
    }
    return NULL;
}
