#include <iostream>
#include <thread>
#include "Thread.hpp"
#include "rw.hpp"

int main(void) {
    std::cout << "start" << std::endl;
    ReadWriteLock lock;
    Data data(10, lock);
    ReadThread rt1(data), rt2(data), rt3(data);
    WriteThread wt1(data, "abcdefghijklmnopqrstuvwxyz");
    WriteThread wt2(data, "ABCDEFGHIJKLMNOPURStUVWXYZ");

    rt1.start(nullptr);
    rt2.start(nullptr);
    rt3.start(nullptr);
    wt1.start(nullptr);
    wt2.start(nullptr);
    
    rt1.wait();
    rt2.wait();
    rt3.wait();
    wt1.wait();
    wt2.wait();
    return 0;
}
