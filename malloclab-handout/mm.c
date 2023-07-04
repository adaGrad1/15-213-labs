/*
malloc implemented via a best-fit single free-block linked-list solution.
I implemented a free-block linked-list solution as I expected it would easily extend to a segregated lists solution
but that ended up not being necessary
I expect you could probably improve performance by reducing the free block sizes by having them only contain their length at the top and bottom

block structure

Filled blocks --
at header: 8-bytes which, if rounded down to the nearest 8, represent the block length as unsigned long.
least significant two bits represent (IS PREV BLOCK FREE) and (IS THIS BLOCK FREE) respectively
Min size: 24 bytes incl header (so a filled block can be converted directly into a free block when freed)

Unfilled blocks --
at header: 
8 byte pointer to next free block with same flags as filled block header
8 byte pointer to prev free block
8 byte unsigned long length
at footer:
8 byte unsigned long length (can be same as header, allowing unfilled block to take only 24 bytes)
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

team_t team = {
    /* Team name */
    "adaboost",
    /* First member's full name */
    "Ada Boost",
    /* First member's email address */
    "asdf@asdf.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define FREELIST_START (*(header_addr))
#define FINAL_BLOCK (*(header_addr+1))



/* 
 * mm_init - initialize the malloc package.
 */

void** header_addr;
int mm_init(void)
{
    mem_sbrk((24) + 16);
    header_addr = mem_heap_lo();
    freeblock_t* fb = ((void*) header_addr) + 16;

    FREELIST_START = (void*) fb;
    FINAL_BLOCK = (void*) fb;
    init_free_block(fb, NULL, NULL, 24, 0);

    return -1*(header_addr == NULL);
}

/*
adds the metadata for a free block beginning at a desired address
length must be a multiple of 8 and be at least 24
*/

void init_free_block(freeblock_t* block_addr, freeblock_t* prev, freeblock_t* next, unsigned long length, char prev_free){
    block_addr->next_block = next;
    block_addr->prev_block = prev;
    block_addr->length = length;
    mark_free(block_addr, 1);
    mark_prev_free(block_addr, prev_free);
    unsigned long* footer_len = (((void*) block_addr)+length-8);
    *footer_len = length;
}

/*
marks a block as free by setting the low bit of the block ptr as 1
*/
void mark_free(void* block_ptr, char is_free){
    if(block_ptr > mem_heap_hi() - 8) return;
    unsigned long val = *((unsigned long*) block_ptr);
    *((unsigned long*) block_ptr) = (val & ~(0x1)) | (!!(is_free));
}

/*
marks prev block as free by setting the second-lowest bit of the block ptr to 1
*/
void mark_prev_free(void* block_ptr, char is_free){
    if(block_ptr > mem_heap_hi() - 8) return;
    unsigned long val = *((unsigned long*) block_ptr);
    *((unsigned long*) block_ptr) = (val & ~(0x2)) | (!!(is_free) << 1);
}

char is_prev_free(void* block_ptr){
    unsigned long val = *((unsigned long*) block_ptr);
    return (val & 0x2) >> 1;
}

char is_free(void* block_ptr){
    unsigned long val = *((unsigned long*) block_ptr);
    return (val & 0x1);
}
/*
returns the value with the low two bits set to 0 (to ignore flag bits when using as a pointer)
*/
freeblock_t* wipe_flags(void* ptr){
    return (void*) (((unsigned long) ptr) & (~0x3));
}

//used to update block pointers while leaving flags intact
void* merge_block_flags(void* ptr, void* flag_ptr){
    unsigned int to_return = ((unsigned int) ptr) & ~(0x3);
    to_return |= (((unsigned int) flag_ptr) & 0x3);
    return (void*) to_return;
}

//will modify this once we make segregated lists
void add_free_to_list(freeblock_t* block_addr){
    block_addr->next_block = merge_block_flags(FREELIST_START, block_addr->next_block);
    freeblock_t* next = FREELIST_START;
    FREELIST_START = block_addr;
    block_addr->prev_block = NULL;
    if(next != NULL)
        next->prev_block = merge_block_flags(FREELIST_START, next->prev_block);
}

void remove_block_from_list(freeblock_t* block){
    freeblock_t* next = wipe_flags(block->next_block);
    freeblock_t* prev = wipe_flags(block->prev_block);
    //removes block from list by connecting prev and next block
    if(next != NULL)
        next->prev_block = merge_block_flags(prev, next->prev_block);
    if(prev != NULL)
        prev->next_block = merge_block_flags(next, prev->next_block);
    if(FREELIST_START == block)
        FREELIST_START = next; //if top of list, we properly maintain the start of the list
}

//mallocs at a particular free block
//sets the first [aligned size + 8] bytes of the block with the 8 bytes as header to the format of a filled block
//sets the remaining bytes, if there are at least 24, to the format of a free block
void* malloc_block(freeblock_t* block_addr, size_t size)
{
    assert(block_addr <= mem_heap_hi() && is_free(block_addr));
    //store prev and next pointers to update list
    //connect prev and next
    remove_block_from_list(block_addr);
    
    unsigned long old_len = block_addr->length;
    char prev_free_flag = is_prev_free(block_addr);
    //set header of new allocated block
    size_t aligned_size = ALIGN(size);
    char dont_split = 0;
    if(block_addr->length - aligned_size < 24) // not enough space after allocation to make a new free block afterwards
    {
        aligned_size = block_addr->length;
        dont_split = 1;
    }
    *((unsigned int*) block_addr) = aligned_size;
    mark_prev_free(block_addr, prev_free_flag);
    mark_free(block_addr, 0);
    //init new free block after allocated block, stick it at the start of the list
    void* next_block_addr = ((void*) block_addr) + aligned_size;
    if(!dont_split){
        if(FINAL_BLOCK == block_addr)
            FINAL_BLOCK = next_block_addr; // if splitting affected the final block, reflect that
        init_free_block(next_block_addr, NULL, NULL, old_len-aligned_size, 0);
        mark_free(next_block_addr, 1);
        add_free_to_list(next_block_addr);
    }
    mark_prev_free(next_block_addr, 0); // newly allocated free block or just next block afterwards
    return ((void*) block_addr)+0x8; // add 8 to miss header
}

freeblock_t* coalesce(freeblock_t* block)
{
    //assumes block's prev and next have not yet been specified

    //if the block that comes after this one is free, store its length and remove it from its respective list
    freeblock_t* next_block = ((void*) block) + block->length;
    unsigned long next_len = 0;
    if((next_block <= mem_heap_hi()) && is_free(next_block)){
        remove_block_from_list(next_block);
        next_len = next_block->length;
    }
    //if the block that comes before this one is free, store its length and prev_free status and remove from its respective list
    unsigned long prev_len = 0;
    freeblock_t* prev_block;
    if(is_prev_free(block)){
        prev_len = *((unsigned long*) (((void*) block) - 8));
        prev_block = ((void*) block) - prev_len;
        remove_block_from_list(prev_block);
    }
    //allocate new free block and put it at the start of the list
    void* new_block_start = ((void*) block) - prev_len;
    unsigned long new_block_len = block->length + prev_len + next_len;
    char prev_free = prev_len > 0 ? is_prev_free(prev_block) : is_prev_free(block);
    init_free_block(new_block_start, NULL, NULL, new_block_len, prev_free); // if we are coalescing properly, then after a coalesce, the prev_free should always be false
    ((freeblock_t*)new_block_start)->next_block = merge_block_flags(FREELIST_START, ((freeblock_t*)new_block_start)->next_block);
    if(*(header_addr) != NULL)
        ((freeblock_t*) FREELIST_START)->prev_block = merge_block_flags(new_block_start, ((freeblock_t*) FREELIST_START)->prev_block);
    FREELIST_START = new_block_start;
    
    //if the one that previously came after this block was the ending block or this block was the ending block, then make this block the ending block
    if(FINAL_BLOCK < new_block_start + new_block_len){
        FINAL_BLOCK = new_block_start;
    }
    //return void because we don't have to use this right away
    assert(is_free(new_block_start));
    return new_block_start;
}

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN_ALLOC_SIZE 24

//TODO init a free block at the end of the range, coalesce it
void* extend_malloc_area(size_t size)
{
    freeblock_t* final_block_header = FINAL_BLOCK;
    unsigned long block_size;
    if(is_free(final_block_header))
        block_size = MAX(MIN_ALLOC_SIZE, ALIGN(size - final_block_header->length));
    else
        block_size = MAX(ALIGN(size), MIN_ALLOC_SIZE);
    if(mem_sbrk(block_size) == (void*)-1)
        return NULL;
    void* new_block = mem_heap_hi()-block_size+1;
    assert((((unsigned int) new_block) & 0x7) == 0);
    init_free_block(new_block, NULL, NULL, block_size, is_free(final_block_header));
    return coalesce(new_block);
}


/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */

/* pseudocode:
    - traverse the free list until we find a block which is large enough
    - populate first free block
    - if there is no large enough free block, allocate more at the end and populate the free block at the end
    - return pointer
*/
void *mm_malloc(size_t size)
{
    size_t aligned_size = MAX(ALIGN(size)+8, 24); //add 8 to account for header space cost. also must be at least 24 in order to work nicely when we free it later
    freeblock_t* cur_addr = FREELIST_START;
    void* to_ret = NULL;
    void* best_block = NULL;
    unsigned long best_size = 1 << 31;
    while (cur_addr != NULL)
    {
        if((cur_addr->length >= aligned_size) && cur_addr->length <= best_size)
        {
            best_block = cur_addr;
            best_size = cur_addr->length;
        }
        cur_addr = wipe_flags(cur_addr->next_block);
    }
    if(best_block != NULL)
        to_ret = malloc_block(best_block, aligned_size);
    else
        to_ret = malloc_block(extend_malloc_area(aligned_size), aligned_size);
    mm_checkheap();
    return to_ret;
}

/*
 * free -- init a free block of the same size as the current filled block, then coalesce
 */
void mm_free(void *ptr)
{
    //printf("free\n");
    ptr = ptr - 0x8;
    unsigned int block_size = (*((unsigned int*)ptr)) & ~(0x7);
    init_free_block(ptr, NULL, NULL, block_size, is_prev_free(ptr));
    if(ptr + block_size <= mem_heap_hi())
        mark_prev_free(ptr + block_size, 1);
    coalesce(ptr);
    mm_checkheap();
    return;
}

void *naive_mm_realloc(void* ptr, size_t size){
    void *oldptr = ptr;
    void *newptr;
    size_t copy_size;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    copy_size = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copy_size)
        copy_size = size;
    memcpy(newptr, oldptr, copy_size);
    mm_free(oldptr);
    return newptr;
}


/*
 */
void *mm_realloc(void *ptr, size_t size)
{
    if(size == 0){
        mm_free(ptr);
        return NULL;
    }
    if(ptr == NULL){
        return mm_malloc(size);
    }
    ptr = ptr - 0x8;
    void* cur_ptr = ptr;
    unsigned long new_len = MAX(24, ALIGN(size + 0x8)); //add 8 bytes to account for header
    unsigned long prev_len = (*(((unsigned long*) cur_ptr))) & (~0x7);
    unsigned long* cur_ptr_long = cur_ptr;
    if(prev_len > new_len + 24) //making it smaller AND we have enough space to allocate a new free block
    {
        *cur_ptr_long = ((*cur_ptr_long) & 0x7) | (new_len & ~(0x7));
        init_free_block(cur_ptr+new_len, NULL, NULL, prev_len-new_len, 0);
        coalesce(cur_ptr+new_len);
        return ptr + 0x8;
    }
    if(new_len > prev_len)
    {
        freeblock_t* next_block_addr = ptr + prev_len;
        //if we're at the end, just resize
        if(FINAL_BLOCK == ptr)
        {
            if(mem_sbrk(new_len - prev_len) == (void*)-1)
                return NULL;
            *cur_ptr_long = ((*cur_ptr_long) & 0x7) | (new_len & ~(0x7));
            return ptr + 0x8;
        } else if(next_block_addr <= mem_heap_hi() && is_free(next_block_addr)) // if we have a free block after this, we may be able to eat into that
        {
            if(next_block_addr->length +prev_len >= new_len){
                remove_block_from_list(next_block_addr);
                unsigned long leftover_space = (new_len - prev_len) - next_block_addr->length;
                if(leftover_space >= 24) // enough space to add a new block!
                    init_free_block(ptr + new_len, NULL, NULL, leftover_space, 0);
                else
                    new_len = new_len + leftover_space;
                *cur_ptr_long = ((*cur_ptr_long) & 0x7) | (new_len & ~(0x7));
                return ptr + 0x8;
            } else if(FINAL_BLOCK == next_block_addr){
                freeblock_t* new_free_block = coalesce(extend_malloc_area(new_len - prev_len));
                remove_block_from_list(new_free_block);
                *cur_ptr_long = ((*cur_ptr_long) & 0x7) | ((prev_len + new_free_block->length) & ~(0x7));
                return ptr+0x8;
            } else{
                return naive_mm_realloc(ptr+0x8, new_len);
            }
        }
        //if there's no free block after and we can't resize, use naive method
        else{
            return naive_mm_realloc(ptr+0x8, new_len);
        }
    }
    return ptr + 0x8;
}


void mm_checkheap(){
    return; // delete return to enable thorough heap checking!
    //go from top to bottom in memory until we pass mem_heap_hi
    //count free blocks, make sure is_free always is the same as next block's is_prev_free
    //make sure we're always block-aligned
    //make sure the free blocks have proper length suffixes
    void* cur_ptr = ((void*) header_addr)+16;
    int free_blocks_ct_1 = 0;
    char prev_block_free = 0;
    while(cur_ptr <= mem_heap_hi()){
        if(is_free(cur_ptr)){
            freeblock_t* cur_ptr_free = cur_ptr;
        }
        assert(prev_block_free == is_prev_free(cur_ptr)); // check is_free consistency
        assert((((unsigned long) cur_ptr) & 0x7) == 0); // check block alignment
        prev_block_free = is_free(cur_ptr);
        if(is_free(cur_ptr)){
            free_blocks_ct_1++;
            unsigned int len1 = ((freeblock_t*) cur_ptr)->length;
            unsigned int len2 = *((unsigned int*)(cur_ptr + len1 - 8));
            assert(len1 == len2); //check that free blocks have proper length suffixes
            cur_ptr = cur_ptr + len1;
        } else{
            cur_ptr = cur_ptr + (*((unsigned long*) cur_ptr) & ~0x3);
        }
    }
    //go in linked list order in memory for free blocks -- make sure the linked list contains as many as the top-down traversal
    //and make sure next->prev is always equal to self
    int free_blocks_ct_2 = 0;
    freeblock_t* cur_list_elem = *((freeblock_t*) FREELIST_START);
    while(cur_list_elem != NULL){
        free_blocks_ct_2++;
        if(wipe_flags(cur_list_elem->next_block) != NULL){
            if(wipe_flags(wipe_flags(cur_list_elem->next_block)->prev_block) != cur_list_elem){
                printf("current block %x has next %x which had prev %x\n", cur_list_elem, wipe_flags(cur_list_elem->next_block), wipe_flags(wipe_flags(cur_list_elem->next_block)->prev_block));
                assert(wipe_flags(wipe_flags(cur_list_elem->next_block)->prev_block) == cur_list_elem);
            }
        }
        cur_list_elem = wipe_flags(cur_list_elem->next_block);
    }
    assert(free_blocks_ct_2 == free_blocks_ct_1);
    //todo -- eventually make sure we never have two free blocks next to each other
    return;
}