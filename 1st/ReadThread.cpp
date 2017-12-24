#include <stdio.h>
#include <unistd.h>
#include "rw.hpp"

ReadThread::ReadThread() {
	this->id = tcount;
}

ReadThread::~ReadThread() {

}

void* ReadThread::run(void* arg) {
    printf("run Thread %d\n", this->id);
	for (int i = 0; i < 5; ++i) {
		printf("work by Thread %d\n", this->id);
		sleep(1);
	}
    return NULL;
}
