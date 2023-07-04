#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <time.h>

#include "mm.h"
#include "memlib.h"
#include "fsecs.h"
#include "config.h"

int verbose = 0; //make gcc shut up


int main(int argc, char **argv){
    mem_init();
    mm_init();
    mm_checkheap();
    //state: size 0x100 free block
    void* a = mm_malloc(3);
    //state: size 0x10 allocated block, 0xF0 free block
    mm_checkheap();
    void* b = mm_malloc(9);
    mm_checkheap();
    //state: size 0x10 allocated block, size 0x18 allocated block, size 0xD8 free block
    void* c = mm_malloc(0x100);
    mm_checkheap();
    void* d = mm_malloc(0x1);
    mm_checkheap();
    return 0;
}