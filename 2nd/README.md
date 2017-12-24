
## C++とPthreadでJavaライクなスレッドライブラリを作る(ロックの導入)

前回に引き続き，今回はスレッド感の同期をとるロックの仕組みを導入する．
pthreadを動悸するには，以下の関数を使う必要がある．

```c
int  pthread_mutex_lock(pthread_mutex_t *mutex);
int  pthread_mutex_unlock(pthread_mutex_t *mutex);
```
そして，この関数を実行する前に初期化，そして使い終わったら削除するクリアする必要がある．

```c
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```

そこで，これらを導入したRWLockクラスを導入する．このクラスの`invoke`は，スレッドを同期して重い処理をさせ，`invoke_without_lock`は，同期せずにスレッドに処理を実行させる．

rw.hpp
```c++
class RWLock {
private:
    pthread_mutex_t mutex; // ロックのための構造体

public:
    RWLock();
    ~RWLock();
    void invoke(int id); // dummy
	void invoke_without_lock(int id);
};
```

RWLock.cpp
```c++
RWLock::RWLock() {
    pthread_mutex_init(&this->mutex, NULL); // コンストラクタで構造体の初期化
}

RWLock::~RWLock() {
    pthread_mutex_destroy(&this->mutex);    // デストラクタで構造体のクリア
}

/* lock test */
void RWLock::invoke(int id) {
    pthread_mutex_lock(&this->mutex);                // ロック
    for (int i = 0; i < 5; ++i) {
        printf("invoked by thread %d(%d)\n", id, i);
        sleep(1);
    }
    pthread_mutex_unlock(&this->mutex);              // アンロック
}

void RWLock::invoke_without_lock(int id) {
   for (int i = 0; i < 5; ++i) {
        printf("invoked by thread %d(%d)\n", id, i);
        sleep(1);
    } 	
}
```

これらの関数を，ReadThreadから使う．

```c++
void* ReadThread::run(void* arg) {
    printf("run Thread %d\n", this->id);
	//lock->invoke(this->id);
	lock->invoke_without_lock(this->id);
    return NULL;
}
```

この例のように，スレッドから``invoke_without_lock``を実行するようにしてmainを実行すると，

main.cpp
```c++
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
```

実行結果は以下のようになる．

```
>./main
run Thread 1
invoked by thread 1(0)
run Thread 2
invoked by thread 2(0)
invoked by thread 1(1)
invoked by thread 2(1)
invoked by thread 1(2)
invoked by thread 2(2)
invoked by thread 1(3)
invoked by thread 2(3)
invoked by thread 1(4)
invoked by thread 2(4)
```
このように，thread1とthread2の出力結果が混在する．
一方，　``invoke``を呼び出すように変更すると，

```c++
void* ReadThread::run(void* arg) {
    printf("run Thread %d\n", this->id);
	lock->invoke(this->id);
	//lock->invoke_without_lock(this->id);
    return NULL;
}
```
実行結果は，
```
run Thread 1
invoked by thread 1(0)
run Thread 2
invoked by thread 1(1) // スレッド1が先にロックを取得
invoked by thread 1(2)
invoked by thread 1(3)
invoked by thread 1(4)
invoked by thread 2(0) // スレッド2は待たされて，個々から実行
invoked by thread 2(1)
invoked by thread 2(2)
invoked by thread 2(3)
invoked by thread 2(4)
```

このように，先にスレッド1がロックを取得し，それをアンロックしてからスレッド2がロックを取得して処理を実行するため，
スレッド1の処理が完了した後スレッド2の処理が始まることがわかる．

全部のソースコードは[こちら](https://github.com/hiro4669/)
