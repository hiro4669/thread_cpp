#include "rw.hpp"
#include <unistd.h>

RWLock::RWLock() {
    pthread_mutex_init(&this->mutex, NULL);
}

RWLock::~RWLock() {
    pthread_mutex_destroy(&this->mutex);
}

/* lock test */
void RWLock::invoke(int id) {
    pthread_mutex_lock(&this->mutex);
    for (int i = 0; i < 5; ++i) {
        printf("invoked by thread %d(%d)\n", id, i);
        sleep(1);
    }
    pthread_mutex_unlock(&this->mutex);
}

void RWLock::invoke_without_lock(int id) {
   for (int i = 0; i < 5; ++i) {
        printf("invoked by thread %d(%d)\n", id, i);
        sleep(1);
    }
}
