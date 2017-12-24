TARGET = main
GXX = g++
CC  = gcc
CFLAGS = -c -g -DDEBUG

OBJS = main.o Data.o ReadThread.o WriteThread.o RWLock.o

all: $(OBJS)
	$(GXX) -o $(TARGET) $^


.cpp.o:
	$(GXX) $(CFLAGS) $*.cpp

.c.o:
	$(CC) $(CFLAGS) $*.c

clean:
	rm -rf $(TARGET) *.o *~

