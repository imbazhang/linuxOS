#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "mem.h"


typedef enum {
	false, true
}bool;
int m_error;
struct memTrack {
	int magic;
	int size;
};

bool isInit = false;

int mem_init(int size_of_region) {
	// called on time by process using our routine
	// size_of_region is the number of bytes 
	// request from OS using mmap()
	int pageSize = getpagesize();
	// in the coding env, the default pageSize is 4096
//	printf("%d\n", pageSize);
	//handle the case that called more than once
	if (isInit) {
		m_error = E_BAD_ARGS;
	}
	// return 0 on success
	// otherwise return -1 and set m_error
	return 0;
}

void *mem_alloc(int size, int style)
{
	// parameter style use macro definition in mem.h
	// best-fit worst-fit first-fit
	
	
	
	// return 8-byte aligned chunks of memory 

}

int mem_free(void *ptr) {

}
void mem_dump() {
	//debug routine
	//print regions of free memory
}

int main ()
{
//	void *tst = mmap();
	int status = mem_init(1024);

	
	return 0;
}
