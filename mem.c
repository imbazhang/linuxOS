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
    printf("%d\n", pageSize);
    //handle the case that called more than once
    if (isInit) {
        m_error = E_BAD_ARGS;
        printf("error : init more than once\n");
        // TODO: error handle
        return -1;
    }
    // return 0 on success
    // otherwise return -1 and set m_error

    int fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        printf("error on open file\n");
        // TODO : fd error handle
        exit(-1);
    }
    struct memTrack *mt;
    size_of_region = (sizeof(mt)+size_of_region - 1)/(pageSize + 1) * pageSize;
    printf("%d\n", size_of_region);

    if ((mem_begin = mmap(NULL, size_of_region, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
    {
        m_error = E_BAD_ARGS;
        printf("error on mmap\n");
        exit(1);
    }
    mem_end = mem_begin + size_of_region;
    close(fd);

    isInit = true;
    printf("%p\n", mem_begin);
    mt = (struct memTrack *)mem_begin;
    printf("%p\n", mem_begin);
    mt -> size = size_of_region;
    mt -> magic = 1;

    return 0;
}

void *mem_alloc(int size, int style)
{
    // parameter style use macro definition in mem.h
    // best-fit worst-fit first-fit
    void *ptr = mem_begin; //pointer
    struct memTrack *trackPtr;
    void *targetLocation = NULL; // return when find location
    printf("%d size represent in 8-byte is ", size);
    size = (((size + sizeof(trackPtr) -1) / 8) << 3) + 8;
    printf("%d\n", size);
    if (style == M_BESTFIT) {
        while (ptr != mem_end) { //iterator all the memory to find best place
            trackPtr = (struct memTrack*)ptr;
            if (trackPtr -> magic && trackPtr -> size >= size) {
                if (targetLocation == NULL || trackPtr -> size < ((struct memTrack*)ptr) -> size)
                    targetLocation = ptr;
            }
            ptr = ptr + trackPtr -> size;
        }
    }
    else if (style == M_FIRSTFIT) {
        while (ptr != mem_end) {
            trackPtr = (struct memTrack*)ptr;
            if (trackPtr -> magic && trackPtr -> size >= size) {
                targetLocation = ptr;
                break;
            }
            ptr = ptr + trackPtr -> size;
        }
    }
    else if (style == M_WORSTFIT) {
        while (ptr != mem_end) { //iterator all the memory to find best place
            trackPtr = (struct memTrack*)ptr;
            if (trackPtr -> magic && trackPtr -> size >= size) {
                if (targetLocation == NULL || trackPtr -> size > ((struct memTrack*)ptr) -> size)
                    targetLocation = ptr;
            }
            ptr = ptr + trackPtr -> size;
        }
        //duplicate from BESTFIT
    }
    if (targetLocation == NULL) {
        m_error = E_NO_SPACE;
        return NULL;
    }

    trackPtr = (struct memTrack *)targetLocation;
    if (trackPtr -> size > size) {
        struct memTrack* next = (struct memTrack *)(targetLocation + size);
        next -> magic = 1;
        next -> size = trackPtr -> size - size;
    }

    trackPtr -> magic = 0;
    trackPtr -> size = size;
    targetLocation = targetLocation + sizeof(struct memTrack);
    printf("return address %p and the control block located in %p\n", targetLocation, targetLocation - sizeof(struct memTrack));
    return targetLocation;
    // return 8-byte aligned chunks of memory
}

int mem_free(void *ptr) {
    struct memTrack *trackPtr = ptr - 8;
    if (ptr == NULL) {
        return -1;
    }
    trackPtr -> magic = 1;
    struct memTrack *next = ptr-8 + trackPtr -> size;
    if (next -> magic == 1) {
        trackPtr -> size = trackPtr -> size + next -> size;
    }

    void *previous = mem_begin;
    struct memTrack *before;
    while (previous < ptr) {
        before = (struct memTrack*)previous;
        if (before->magic == 1 && previous + before -> size == ptr - 8) {
            before->size = before->size + trackPtr->size;
            break;
        }
        previous += (before->size);
    }
    mem_dump();
    return 0;
}
void mem_dump() {
    printf("start dump!\n");
    //debug routine
    //print regions of free memory
    //
    void *ptr = mem_begin;
    struct memTrack *trackPtr;
    while (ptr != mem_end) {
        trackPtr = (struct memTrack*)ptr;
        if (trackPtr -> magic == 1) {
            printf("%p - %p, the size is %d, this chunk is available\n", ptr, ptr+trackPtr -> size, trackPtr -> size);
        }
        else {
            printf("%p - %p, the size is %d, this chunk is not available\n", ptr, ptr+trackPtr -> size, trackPtr -> size);
        }
        ptr = ptr + trackPtr -> size;
    }
    printf("end dump!\n");
}

int main ()
{
//	void *tst = mmap();
    int status = mem_init(4096);
    mem_dump();

    char *p1, *p2, *p3, *p4, *p5;
    p1 =(char*)mem_alloc(10*sizeof(char),2);
    //printf("Need: %d, actual: %d\n", 10 * sizeof(char), ((struct  memTrack*)(p1 - sizeof(struct trackPtr)) ->size));
    printf("dump p1\n");
    mem_dump();
    p2 =(char*)mem_alloc(20*sizeof(char),1);
    printf("dump p2\n");
    mem_dump();
    p3 =(char*)mem_alloc(30*sizeof(char),1);
    printf("dump p3\n");
    mem_dump();
    p4 =(char*)mem_alloc(36*sizeof(char),1);
    printf("dump p4\n");
    mem_dump();
    mem_free(p1);
    mem_free(p3);
    p5 = (char*) mem_alloc(5*sizeof(char),1);
    mem_dump();
    mem_free(p4);
    mem_free(p2);
    mem_free(p5);
    return 0;
}
