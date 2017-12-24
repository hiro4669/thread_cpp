#include <stdio.h>
#include <unistd.h>


#include "rw.hpp"
#include "static.hpp"


int main(void) {
    RWLock lock;
    ReadThread rt1(&lock), rt2(&lock);

    rt1.start(NULL);
    usleep(500000); // 0.5sec wait
    rt2.start(NULL);

    
    while(true) {
        sleep(1);
    }  
    return 0;
}
