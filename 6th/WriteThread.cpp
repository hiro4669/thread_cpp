#include <iostream>
#include <unistd.h>
#include "rw.hpp"

WriteThread::WriteThread(Data& d, const char *s) : data(d) {
    this->filter = s;
    this->fil_len = strlen(this->filter);
    this->index = 0;
    srand(120);
}

WriteThread::~WriteThread() {
}

char WriteThread::nextChar() {
    char c = this->filter[this->index++];
    if (this->index >= this->fil_len) {
        this->index = 0;
    }
    return c;
}

void WriteThread::run(void* arg) {
    printf("run WriteThread %d\n", this->id);
    while (true) {
        char c = nextChar();
        data.write(c);
        usleep(rand() % 3 * 1000000);
    }    
}