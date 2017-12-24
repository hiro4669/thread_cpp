## C++とPthreadでJavaライクなスレッドライブラリを作る(Writeスレッド追加)


前回，lock変数とpthreadのmutexを使ってロック/通知の仕組みを実装し，ReadThreadが正しく動作することを確認したので，今回はWriteThreadを追加して，この仕組がうまく動くことを確認する．そこで，以下のクラスを実装する．

- Data：ロックする`read/write`メソッド，ロックしない`unlockRead/unlockWrite`メソッドを持ち，read系のメソッドはある変数(`num`)を100回デクリメント，write系のメソッドは300回インクリメントするメソッド，doReadTest/doWriteTestを内部で呼び出す．
- WriteThread: write系のメソッドを呼び出す．
- ReadThread:  read系のメソッドを呼び出す．

rw.hpp
```c++
class Data {
private:
    int num;
    int bufSize;
    RWLock *lock;

    void slowly();

    /* for test */
    int doReadTest();
    int doWriteTest();


public:
    Data();
    Data(int size, RWLock *lock);
    ~Data();

    int getBufSize();

    /* for test */
    int read(int id);
    int unlockRead(int id);
    int write(int id);
    int unlockWrite(int id);
};
```

main.cpp
```c++
int main(void) {
    std::string s;
    RWLock lock;
    Data d(&lock);
    ReadThread rt1(&d), rt2(&d), rt3(&d);  // ReadThreadを3つ生成
    WriteThread wt1(&d);                   // WriteThreadを1つ生成
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

```

まず，Read/Writeスレッドともに，ロックしないメソッドを呼び出すようにして実行してみる．

```c++
void* ReadThread::run(void* arg) {
    printf("run Thread %d\n", this->id);
    int num = this->pData->unlockRead(this->id);   --> ここ
    printf("ReadThread%d num = %d\n", this->id, num);
    return NULL;
}

void* WriteThread::run(void* arg) {
    printf("run WriteThread %d\n", this->id);
    int num = this->pData->unlockWrite(this->id);　--> ここ
    printf("WriteThread%d num = %d\n", this->id, num);
    return NULL;
}
```

すると，以下の結果を得る．
```
run Thread 1
run Thread 2
run Thread 3
run WriteThread 4
ReadThread2 num = -183
ReadThread3 num = -186
ReadThread1 num = -182
WriteThread4 num = 2
```
ロックしないことによって，最終的にnumが0にならない

次に，ロックするread/writeに変更する

```c++
void* ReadThread::run(void* arg) {
    printf("run Thread %d\n", this->id);
    int num = this->pData->read(this->id);   --> ここ
    printf("ReadThread%d num = %d\n", this->id, num);
    return NULL;
}

void* WriteThread::run(void* arg) {
    printf("run WriteThread %d\n", this->id);
    int num = this->pData->write(this->id);　--> ここ
    printf("WriteThread%d num = %d\n", this->id, num);
    return NULL;
}
```
すると，

```
run Thread 1
run Thread 2
run Thread 3
run WriteThread 4
read lock by 1
get lock by 1
read lock by 2
lock failed wait by 2
read lock by 3
lock failed wait by 3
write lock by 4
lock failed wait by 4
read unlock by 1
ReadThread1 num = -100
finish wait by 2
get lock by 2
finish wait by 3
lock failed wait by 3
finish wait by 4
lock failed wait by 4
read unlock by 2
ReadThread2 num = -200
finish wait by 3
get lock by 3
finish wait by 4
lock failed wait by 4
read unlock by 3
ReadThread3 num = -300
finish wait by 4
get lock by 4
write unlock by 4
WriteThread4 num = 0  -> ここ
```
最終的にnumは0になる．
つまり，うまく動いていることがわかる．

全部のソースコードは[こちら](https://github.com/hiro4669/)
