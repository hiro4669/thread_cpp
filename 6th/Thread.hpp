
#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#include <thread>

class Thread {
private:
    void* args;
    static void wrap(Thread* pt) {
        pt->run(pt->args);
    }
    
protected:
    std::thread t;    
    std::thread::id getId() {
        return t.get_id();
    }

    
public:
    Thread(){}
    virtual ~Thread(){}
    virtual void run(void* arg) = 0;
    
    void start(void* args) {
        this->args = args;
        t = std::thread(Thread::wrap, this);
    }
    
    void wait() {
        t.join();
    }
};



#endif /* THREAD_HPP */

