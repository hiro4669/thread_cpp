#include <stdio.h>
#include <unistd.h>


#include "rw.hpp"
#include "static.hpp"


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
