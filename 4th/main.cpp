#include <stdio.h>
#include <unistd.h>


#include "rw.hpp"
#include "static.hpp"

int main(void) {
    std::string s;
    RWLock lock;
    Data d(&lock);
    ReadThread rt1(&d), rt2(&d), rt3(&d);
    WriteThread wt1(&d);
    rt1.start(NULL);
    rt2.start(NULL);
    rt3.start(NULL);
    wt1.start(NULL);

    while(true) {
        sleep(1);
//        printf("sleep...\n");
    }
    return 0;
}
