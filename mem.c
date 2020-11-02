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
void *mem_begin;
void *mem_end;	
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
		return -1;
	}
	// return 0 on success
	// otherwise return -1 and set m_error
	//
	int fd = open("dev/zero", O_RDWR);
	struct memTrack *mt; 
	size_of_region = (sizeof(mt)+size_of_region - 1)/(pageSize + 1) * pageSize;
	printf("%d\n", size_of_region);
	
	if ((mem_begin = mmap(NULL, size_of_region, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
	{
		m_error = E_BAD_ARGS;
		exit(1);
	}
	mem_end = mem_begin + size_of_region;
	close(fd);

	isInit = true;
	printf("%p\n", mem_begin);
	mt = (struct memTrack *)mem_begin;
	printf("%p\n", mem_begin);
	mt -> size = size_of_region;mt -> magic = 1;

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
	//
	void *ptr = mem_begin;
	struct memTrack *trackPtr;
	while (ptr != mem_end) {
		trackPtr = (struct memTrack*)trackPtr;
	}
}

int main ()
{
//	void *tst = mmap();
	int status = mem_init(1024);
	mem_dump();
	
	return 0;
}
