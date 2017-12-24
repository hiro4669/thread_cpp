
### C++とPthreadでJavaライクなスレッドライブラリを作る~その１ - Threadクラス

ここでは，C++でRead/Write Lockデザインパターンを実装すべく，Javaのスレッドライブラリ風なAPIを作っていく．Threadクラスを継承して使う設計にする．

まず，pthread_createの定義は

```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
         void *(*start_routine)(void *), void *arg);
```

であり，第3引数にvoid*を引数で受取，void*を返す関数ポインタを指定する必要がある．そして，第4引数は
第3引数の関数へ与える引数のためのvoidポインタを指定することになっている．
今回，Javaのスレッドライブラリのように，`new Thread().start()`でThreadを継承するクラスのrunを実行する．
ところが，この第3引数に与える関数ポインタは，スタティックじゃなければいけないので，そのまま`run`をわたす訳にはいかない．そこで，`Thread`にstaticな`wrap`メソッドを用意し，これ経由でインスタンスメソッドである`run`を実行するようにした．

Thread.hpp
```c++
class Thread {
private:
    pthread_t thread;   // pthreadのための構造体
    void *args;         // runの引数のためのポインタ

protected:
    static int tcount;

public:
    Thread(){tcount++; /*printf("thread\n");*/}
    virtual ~Thread() {}
    virtual void* run(void *arg) = 0;

    static void* wrap(void *arg) {
        Thread* pt = (Thread*)arg;  // 第4引数で自分自身(this)が渡されるので，キャストしてrunを実行
        return pt->run(pt->args);
    }
    void start(void *arg) {
        this->args = arg;
        if (pthread_create(&this->thread, NULL, wrap, this)) {  // スレッドの生成. 第4引数にthisを指定
            printf("thread create error\n");
            abort();
        }
    }

    void wait() {
        if (pthread_join(this->thread, NULL)) {
            printf("join error");
            abort();
        }
    }
};
```

続いて，Threadを継承する`ReadThread`．これは単に，スレッドIDとともに1秒ごとに"work by Thread ..."と表示するだけ．
なお，スレッドIDとして利用している`tcount`はクラス変数であるため，どこかで実体を定義する必要がある．

rw.hpp
```c++
include "Thread.hpp"

class ReadThread : public Thread {
private:
    int id;

public:

    ReadThread();
    ~ReadThread();
    void* run(void *arg);
};
```
ReadWrite.cpp
```c++
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
```
次は，これを利用するmain.cpp

main.cpp
```c++
int main(void) {
    ReadThread rt1, rt2;

	rt1.start(NULL);
	usleep(500000);
	rt2.start(NULL);

	rt1.wait();
	rt2.wait();

    printf("end of mein thread\n");

    return 0;
}
```

実行結果
```
> ./main
run Thread 1
work by Thread 1
run Thread 2
work by Thread 2
work by Thread 1
work by Thread 2
work by Thread 1
work by Thread 2
work by Thread 1
work by Thread 2
work by Thread 1
work by Thread 2
end of mein thread
```
個々のスレッドか交互ログが出力されていることわかる．
