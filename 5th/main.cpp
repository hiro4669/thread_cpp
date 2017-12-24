#include <stdio.h>
#include <unistd.h>

#include "rw.hpp"
#include "static.hpp"

int main(void) {
    std::string s;
    ReadWriteLock lock;

    Data d(10, &lock);
    ReadThread rt1(&d), rt2(&d), rt3(&d);
    WriteThread wt1(&d, "abcdefghijklmnopqrstuvwxyz");
    WriteThread wt2(&d, "ABCDEFGHIJKLMNOPURStUVWXYZ");

    rt1.start(NULL);
    rt2.start(NULL);
    rt3.start(NULL);
    wt1.start(NULL);
    wt2.start(NULL);

    while(true) {
        sleep(1);
//        printf("sleep...\n");
    }
    return 0;
}
