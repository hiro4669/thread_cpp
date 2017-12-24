#include "rw.hpp"
#include <unistd.h>

WriteThread::WriteThread() : pData(NULL), filter("") {
}

WriteThread::WriteThread(Data* data, const char *s) {
    this->id = tcount;
    this->pData = data;
    this->filter = s;
    this->fil_len = strlen(this->filter);
    this->index = 0;
    srand(120);
    printf("%s(%d)\n", this->filter, this->fil_len);
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

void* WriteThread::run(void* arg) {
    printf("run WriteThread %d\n", this->id);
    while (true) {
        char c = nextChar();
        this->pData->write(c);
        usleep(rand() % 3 * 1000000);
    }
    return NULL;
}
