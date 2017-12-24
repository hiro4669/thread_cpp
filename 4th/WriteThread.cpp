/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "rw.hpp"


WriteThread::WriteThread() : pData(NULL), filter("") {    
}

WriteThread::WriteThread(Data* data) {
    this->id = tcount;
    this->pData = data;
//    this->filter = s;
}

WriteThread::~WriteThread() {    
}

void* WriteThread::run(void* arg) {
    printf("run WriteThread %d\n", this->id);

//    int num = this->pData->unlockWrite(this->id);
    int num = this->pData->write(this->id);    
    printf("WriteThread%d num = %d\n", this->id, num);
    
    
    return NULL;
}

