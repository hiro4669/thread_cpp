## C++とPthreadでJavaライクなスレッドライブラリを作る(Read-Write Lockパターン)

ここまでのクラスを拡張し，デザインパターンの１つであるRead-Write Lockパターンを実装していく．
Read-Write Lockパターンとは，簡単に言うとReadとWriteの競合を回避するため，

- Write-Write競合は防ぐ
- Read-Write競合は防ぐ
- Read-Read競合は許す．

と，読み込み競合を許容する事によってパフォーマンスを上げるパターンです．
今回登場するクラスは以下の通り．

- Data: 文字列のバッファを持ち，ReaderとWriterからアクセスされる．
- ReadWriteLock: 競合を解決するためにロック機能を提供する．
- ReadThread: Dataのバッファに対して文字を読み出す．
- WriteThread: Dataのバッファ位に対して文字を書き込む．

ここで，WriteThreadは，**指定した文字でDataのバッファをすべて埋め尽くす**，という動作をします．一方，ReaderThreadは，**バッファの内容をすべてコピーし，内容を表示します**．もしWrite-Write競合やRead-Write競合が起きてしまうと，バッファを読んでいる途中で書き込みが起こり，読み出したバッファの一部が異なる文字になってしまいます．同様にWrite-Write競合が起きていると，ある文字でバッファを書いている途中で他の文字が書き込まれることになり，バッファの内容が一部変更されてしまうことになります．そのため，**ReadThreadで読み出し，表示するバッファの内容がすべて同じ文字であることが，Read-Write Lockパターンの実装の成功**を意味します．

ではまず，クラス定義一覧
```c++


class ReadWriteLock {
private:
    pthread_mutex_t mutex;
    pthread_cond_t mcond;
    int readingReaders;
    int waitingWriters;
    int writingWriters;
    bool preferWriter;

public:
    ReadWriteLock();
    ~ReadWriteLock();
    void readLock();
    void readUnlock();
    void writeLock();
    void writeUnlock();
};

class Data {
private:
    int num;
    int bufSize;
    char *buffer;
    ReadWriteLock *lock;

    void slowly();
    char* doRead();
    void doWrite(char c);

public:
    Data();
    Data(int size, ReadWriteLock *lock);
    ~Data();

    char* read();
    void write(char c);
    int getBufSize();
};


class ReadThread : public Thread {
private:
    Data *pData;
    int id;
    void lock_unlock_test();
public:

    ReadThread();
    ReadThread(Data *data);
    ~ReadThread();
    void* run(void *arg);
    void showbuf(char* buf, int size);
};

class WriteThread : public Thread {
private:
    Data *pData;
    int id;
    const char* filter;
    int fil_len;
    int index;

    char nextChar();

public:
    WriteThread();
    WriteThread(Data *data, const char *s);
    ~WriteThread();
    void* run(void *arg);
};
```

Dataクラスです．コンストラクタでバッファ(`buffer`)を生成し，初期値として”*”を代入しています．そして，read()/write()メソッドを提供し，その内部ではReadWriteLockクラスを使ってロックしながら読み書きを行います．
```c++
Data::Data(int size, ReadWriteLock *lock) {
    srand(100);
    this->num = 0;
    this->lock = lock;
    this->buffer = new char[this->bufSize = size];
    printf("data constructor with size\n");
    for (int i = 0; i < this->bufSize; ++i) {
        this->buffer[i] = '*';
    }
}
Data::~Data() {
    printf("data denstructor\n");
    delete[] this->buffer;
}

char* Data::read() {
    lock->readLock();
    char* data = doRead();
    lock->readUnlock();
    return data;
}

char* Data::doRead() {
    char *newbuf = new char[this->bufSize];
    for (int i = 0; i < this->bufSize; ++i) {
        newbuf[i] = this->buffer[i];
    }
    this->slowly();
    return newbuf;
}

void Data::write(char c) {
    lock->writeLock();
    this->doWrite(c);
    lock->writeUnlock();
}

void Data::doWrite(char c) {
    for (int i = 0; i < this->bufSize; ++i) {
        this->buffer[i] = c;
        this->slowly();
    }
}

void Data::slowly() {
    usleep(50000);
}

int Data::getBufSize() {
    return this->bufSize;
}
```

次に，ReadThreadクラスの主要部分です．スレッドが実行されると，Dataクラスのreadメソッド(`pDAta->read()`)を実行し，その結果を表示しています(`this->showbuf()`)．
```c++

void ReadThread::showbuf(char *buf, int size) {
    printf("ReadThread %d read ", this->id);
    for (int i = 0; i < size; ++i) {
        printf("%c", buf[i]);
    }
    printf("\n");
}

void* ReadThread::run(void* arg) {
    printf("run Thread %d\n", this->id);
    while(true) {
        char* buf = this->pData->read();
        this->showbuf(buf, this->pData->getBufSize());
        free(buf);
    }
    return NULL;
}
```

次に，WriteThreadクラスの主要部分です．スレッドが実行されると，Dataクラスのwriteメソッド(pData->write(c))を実行し，バッファに文字を書き込みます．なお，書き込む文字はコンストラクタで渡します．
```c++

char WriteThread::nextChar() {
    char c = this->filter[this->index++];
    if (this->index >= this->fil_len) {
        this->index = 0;
    }
    return c;
}

void* WriteThread::run(void* arg) {
    printf("run WriteThread %d\n", this->id);
    while (true) {
        char c = nextChar();
        this->pData->write(c);
        usleep(rand() % 3 * 1000000);
    }
    return NULL;
}
```
最後に，このパターンの肝である，ReadWriteLockクラスを示します．
```c++
eadWriteLock::ReadWriteLock() {
    this->readingReaders = 0;
    this->waitingWriters = 0;
    this->writingWriters = 0;
    this->preferWriter = true;

    pthread_mutex_init(&this->mutex, NULL);
    pthread_cond_init(&this->mcond, NULL);    
}

ReadWriteLock::~ReadWriteLock() {
    pthread_mutex_destroy(&this->mutex);
    pthread_cond_destroy(&this->mcond);
}

void ReadWriteLock::readLock() {
    pthread_mutex_lock(&this->mutex);
    while (writingWriters > 0 || (preferWriter && waitingWriters > 0)) { --> (1)
        pthread_cond_wait(&this->mcond, &this->mutex);
    }
    this->readingReaders++;    
    pthread_mutex_unlock(&this->mutex);    
}

void ReadWriteLock::readUnlock() {
    pthread_mutex_lock(&this->mutex);
    this->readingReaders--;                                              --> (2)
    this->preferWriter = true;
    pthread_cond_broadcast(&this->mcond);
    pthread_mutex_unlock(&this->mutex);    
}

void ReadWriteLock::writeLock() {
    pthread_mutex_lock(&this->mutex);
    this->waitingWriters++;
    while (readingReaders > 0 || writingWriters > 0) {                   --> (3)
        pthread_cond_wait(&this->mcond, &this->mutex);
    }
    this->waitingWriters--;
    this->writingWriters++;
    pthread_mutex_unlock(&this->mutex);    
}

void ReadWriteLock::writeUnlock() {
    pthread_mutex_lock(&this->mutex);
    this->writingWriters--;                                              --> (4)
    this->preferWriter = false;
    pthread_cond_broadcast(&this->mcond);
    pthread_mutex_unlock(&this->mutex);    
}
```

前回は`lock`変数で読み書き競合の解決を図っていましたが，今回は以下の4つの変数を使っています．

- readingReaders: 読み込みを行っているreaderの数
- waitingWriters: 書き込みを行いたいが，ロックが取れずにロックの解放を待っているwriterの数
- writingWriters: 現在書いているwriterの数を表す(高々１のはず)．
- preferWriter  : 読み込みと書き込みが交互に起こることを制御するフラグ．これがないと，読み込みばかり起こる可能性がある(読み込み側が多い場合)．

ロックの制御は前回と変わりませんが，４つの変数を使って制御しています．

(1)のreadLockのところでは，`writingWriters > 0 || (preferWriter && waitingWriters > 0)`なので，書いているスレッドがいる(`writingWriters>0`)，または書き込みを待っているスレッドがいる(`waitingWriters>0`)かつ書き込み優先(`preferWriter == true`)の場合，読み込みを待ちます．

(2)のreadUnlockでは，読み込みが終わったので，`readingReader`をデクリメントし，`preferWriter`を書き込み優先にします．これにより，読み込みがのロックが連続することを防ぎます．

(3)のwriteLockでは，読み込みスレッドがいない(`readingReaders > 0`)．かつ書き込みスレッドがいれば(writingWriters > 0)，書き込みを待ちます．

(4)のwriteUnlockでは，`writingWriters`をデクリメントし，`preferWriter`を読み込み優先にします．これにより，書き込みがのロックが連続することを防ぎます．

以上を実装し，実行すると以下の結果を得ます．
```
ReadThread 1 read dddddddddd
ReadThread 2 read dddddddddd
ReadThread 3 read dddddddddd
ReadThread 1 read dddddddddd
ReadThread 2 read ReadThread 3 read dddddddddd
dddddddddd
ReadThread 1 read dddddddddd
ReadThread 3 read dddddddddd
ReadThread 2 read dddddddddd
ReadThread 1 read dddddddddd
ReadThread 2 read dddddddddd
ReadThread 3 read dddddddddd
ReadThread 1 read dddddddddd
ReadThread 2 read ReadThread 3 read dddddddddddddddddddd

ReadThread 1 read HHHHHHHHHH
ReadThread 3 read HHHHHHHHHH
```
ちょっとわかりにくいですが，今回バッファとして10バイト取っているので，ReaderThreadが出力する文字は10文字単位となります．Read-Readロックは行っていないので，出力は崩れていますが，10文字出力されるうち途中で1文字も異なる文字が出てこない，ということがうまくいっている証拠となります．なお，このプログラムでは，10文字出力単位で数ミリ秒，1文字書き込み単位で数ミリ秒スリープするので，書き込みが起こっているときに画面の出力が止まることも確認できます．

全部のソースコードは[こちら](https://github.com/hiro4669/)
