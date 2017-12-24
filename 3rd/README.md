
## C++とPthreadでJavaライクなスレッドライブラリを作る(ロックと通知)

前回導入したロックを使うことで，スレッド間の同期ができるようにはなった．
しかし，ロックの仕組みだけを使うと，スレッドは次のような動作する．ここでは，スレッド1とスレッド2が存在し，お互いにロックを取ってから処理することを考える．

1. スレッド1がロックを取ろうとする
1. スレッド2がロックと取ろうとする
1. スレッド1がロックを取り，処理をする．
1. スレッド2はロックが取れないため，**待つ**
1. スレッド1の処理が終わり，ロックを開放する
1. スレッド2がロックを取り，処理をする
1. スレッド2が処理を終わり，ロックを開放する

ここで，4の**待つ**というところに着目する．このときスレッド2はCPUを使って待っている(多分)．つまり，ロックが獲得できず，処理が進められないにも関わらず，リソースを使っているということになる．これは明らかに無駄である．そこで，次のようにできると便利である．

1. スレッド1がロックを取ろうとする
1. スレッド2がロックと取ろうとする
1. スレッド1がロックを取り，処理をする．
1. スレッド2はロックが取れないため，**待つ**．そして，休止
1. スレッド1の処理が終わり，ロックを開放し，**スレッド2に終わったことを通知する**．
1. **スレッド2は通知を受けて**，ロックを取り処理をする
1. スレッド2が処理を終わり，ロックを開放する



これを実現するために，pthreadの以下の機能を利用する
```c
int  pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int  pthread_cond_broadcast(pthread_cond_t *cond);
int  pthread_cond_signal(pthread_cond_t *cond);
```
`pthread_cond_wait`は，次のように動作する．
1. 取得しているロックを開放する
1. `mutex`で管理されるロックに関し，`cond`をモニターにして，`cond`の状態に変化があるまでスリープ

この結果，`pthread_cond_wait`を実行したスレッドは休止する，すなわちCPUを使わない．
次に，`pthread_cond_broadcast`は，`cond`をモニターにしてスリープしているスレッドを全員起こす，という機能を持ち，`pthread_cond_signal`は，`cond`をモニターにしてスリープしているスレッドのうち1つだけ起こす，という機能を持つ．

これらを利用し，より効率の良いロックを実現する．

rw.hpp
```c++
class RWLock {
private:
    int readCount;
    pthread_mutex_t mutex;  // pthread_mutex用の構造体
    pthread_cond_t  mcond;  // モニタ用の構造体
    bool lock;              // ロック管理のための変数

public:
    RWLock();
    ~RWLock();
    void readLock(int id);
    void readUnlock(int id);
};
```

RWLock.cpp
```c++
RWLock::RWLock() {
    this->lock = true;                 
    pthread_mutex_init(&this->mutex, NULL);
    pthread_cond_init(&this->mcond, NULL);
}

RWLock::~RWLock() {
    pthread_mutex_destroy(&this->mutex);
    pthread_cond_destroy(&this->mcond);
}

void RWLock::readLock(int id) {
    pthread_mutex_lock(&this->mutex);             -> (1)
    printf("read lock by %d\n", id);
    while (!this->lock) {                         -> (2)
        printf("lock failed wait by %d\n", id);
        pthread_cond_wait(&this->mcond, &this->mutex); -> (3)
        printf("finish wait by %d\n", id);
    }
    this->lock = false;                           -> (4)
    printf("get lock by %d\n", id);
    pthread_mutex_unlock(&this->mutex);           -> (5)
}

void RWLock::readUnlock(int id) {
    pthread_mutex_lock(&this->mutex);
    printf("read unlock by %d\n", id);
    this->lock = true;
    pthread_cond_broadcast(&this->mcond);         -> (6)
    pthread_mutex_unlock(&this->mutex);
}
```
今までの実装が，`pthread_mutex_lock/unlock`でロック/アンロックを制御していたのに対し，今回はそれらの関数を使いつつ，ロック獲得の有無を自分で管理する(`lock`変数)ことになる．

readLockを詳しく見る．
(1)
```c++
pthread_mutex_lock(&this->mutex);
```
`mutex`を使ってpthreadのロックを獲得する

(2)
```c++
while(!this->lock)
```
`lock`変数は，`true`のときロック獲得可能，`false`のときロック獲得不可能を意味している．初期化処理で`true`に設定されるので，ロックが獲得できるときはこの`while`には入らず，(4)にスキップする．

(3)
```c++
pthread_cond_wait(&this->mcond, &this->mutex)
```
ここに入ったということは，ロックが獲得できなかったことを意味している．すなわち，`lock`が`false`であったためである．そこで，このスレッドは**ただちにpthreadのロックを開放**し，mcondをモニターに休眠する．その結果，仮に別のスレッドがこの関数(`readLock`)を呼び出したときにも，(1)でpthreadのロックを獲得することが可能になる．

(4)
```c++
his->lock = false
```
ここでロックを獲得する．つまり，この変数の値を読み書きするときにpthreadのロック/アンロックを使用していることになる．

(5)
```c++
pthread_mutex_unlock(&this->mutex)
```
これでpthreadのロックを開放する．つまり，ロックを獲得できるスレッドは，
1. pthreadのロックを獲得
1. lock変数の変更
1. pthreadのロックを開放

をすることで，ロック(`lock`変数)を獲得していることになる．

一方，ロックを獲得できないスレッドは，
1. pthreadのロックを獲得．その結果失敗．
1. pthreadのロック解放後，`mcond`をモニタにした休眠

となる．

次に，readUnlockを詳しく見る．
`readUnlock`は，pthreadのロックを獲得した後，`lock`変数を変更し，pthreadのロックを開放している．ただし，`lock`変数変更後，
(6)
```c++
thread_cond_broadcast(&this->mcond)
```
を実行している．この結果，(3)で休眠していたスレッドのいずれか一つが起こされる．
起きる対象に選択されたスレッドは，実行を再開する前に`pthread_mutex_lock(&this->mutex)`を再実行したあと実行を再開する(これはpthreadライブラリが内部でやる)．従って，他のスレッドは`lock`を変更することができず，起きる前に`lock`は`true`に設定されるので，(2)の`while`には入らず，(4)に処理が遷移する．そして(5)が実行され，pthreadのロックが解放される．


動作を確認するために，次のプログラムを実行する

ReadThread.cpp
```c++
ReadThread::ReadThread(RWLock *lock) {
    this->id = tcount;
    this->lock = lock;
}
ReadThread::~ReadThread() {    
}

void ReadThread::lock_unlock_test() {
    lock->readLock(this->id);
    for (int i = 0; i < 5; ++i) {
        printf("heavy job by %d(%d)...\n", this->id, i);
        usleep(500000);
    }
    lock->readUnlock(this->id);
}

void* ReadThread::run(void* arg) {
    printf("run Thread %d\n", this->id);
    this->lock_unlock_test();
    return NULL;
}
```

このスレッドは，ロックを獲得して重い処理を行い，ロックを開放する関数，`lock_unlock_test`を実行する

main.cpp
```c++
int main(void) {
    RWLock lock;
    ReadThread rt1(&lock), rt2(&lock), rt3(&lock);

    rt1.start(NULL);
    usleep(300000); // 0.5sec wait
    rt2.start(NULL);
    usleep(300000); // 0.5sec wait
    rt3.start(NULL);

    while(true) {
        sleep(1);
    }    
    return 0;
}
```
mainでは，スレッドを3つ作成し，少しずつdelayさせて順番に実行する．そのため，先に実行したスレッドが最初にロック獲得できるはずである．

実行結果
```
>./main
run Thread 1            // スレッド1実行開始
read lock by 1          // スレッド1がロック獲得を試みる
get lock by 1           // スレッド1がロックを獲得
heavy job by 1(0)...
run Thread 2            // スレッド2実行開始
read lock by 2          // スレッド2がロック獲得を試みる
lock failed wait by 2   // スレッド2がロック獲得失敗し，休眠
heavy job by 1(1)...    // スレッド1の実行
run Thread 3            // スレッド3実行開始
read lock by 3          // スレッド3がロック獲得を試みる
lock failed wait by 3   // スレッド3がロック獲得失敗し，休眠
heavy job by 1(2)...    // スレッド1しか実行できるスレッドがないため，そのまま実行を継続
heavy job by 1(3)...
heavy job by 1(4)...
read unlock by 1        // スレッド1がロックを開放
finish wait by 2        // スレッド2が起こされる．
get lock by 2           // スレッド1がロックを獲得
heavy job by 2(0)...    
finish wait by 3        // スレッド3が起こされる．
lock failed wait by 3   // スレッド3がロック獲得失敗し，休眠
heavy job by 2(1)...    // スレッド2しか実行できるスレッドがないため，そのまま実行を継続
heavy job by 2(2)...
heavy job by 2(3)...
heavy job by 2(4)...
read unlock by 2
finish wait by 3
get lock by 3
heavy job by 3(0)...
heavy job by 3(1)...
heavy job by 3(2)...
heavy job by 3(3)...
heavy job by 3(4)...
read unlock by 3
```

全部のソースコードは[こちら](https://github.com/hiro4669/)
