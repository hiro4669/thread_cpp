TARGET = main
GXX = g++
CC  = gcc
CFLAGS = -c -g -std=c++11 -DDEBUG

OBJS = main.o ReadThread.o WriteThread.o Data.o ReadWriteLock.o

all: $(OBJS)
	$(GXX) -o $(TARGET) $^

.cpp.o:
	$(GXX) $(CFLAGS) $*.cpp

.c.o:
	$(CC) $(CFLAGS) $*.c

clean:
	rm -rf $(TARGET) *.o *~

