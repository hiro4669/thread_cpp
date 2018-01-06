#include "rw.hpp"
#include "Thread.hpp"
#include <iostream>

ReadThread::ReadThread(Data& d) : data(d) {
}
ReadThread::~ReadThread() {
}

void ReadThread::showbuf(char *buf, int size) {
    std::cout << "ReadThread " << getId() << " read " << std::endl;    
    for (int i = 0; i < size; ++i) {
        printf("%c", buf[i]);
    }
    printf("\n");
}

void ReadThread::run(void* arg) {
    std::cout << "run Thread " << getId() << std::endl;    
    while(true) {
        char* buf = data.read();
        this->showbuf(buf, data.getBufSize());
        free(buf);
    }    
}