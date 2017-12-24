#include <stdio.h>
#include <unistd.h>


#include "rw.hpp"
#include "static.hpp"

int main(void) {
    RWLock lock;
    ReadThread rt1(&lock), rt2(&lock), rt3(&lock);

    rt1.start(NULL);
    usleep(300000); // 0.3sec wait
    rt2.start(NULL);
    usleep(300000); // 0.3sec wait	
    rt3.start(NULL);

    while(true) {
        sleep(1);
    }

    return 0;
}
