#include <stdio.h>

struct freeblock {
    struct freeblock* next_block;
    struct freeblock* prev_block;
    unsigned long length;
};
#define freeblock_t struct freeblock


extern int mm_init (void);
extern void *mm_malloc (size_t size);
extern void mm_free (void *ptr);
extern void *mm_realloc(void *ptr, size_t size);
extern void mm_checkheap(void);
void init_free_block(freeblock_t* block_addr, freeblock_t* prev, freeblock_t* next, unsigned long length, char prev_free);






/* 
 * Students work in teams of one or two.  Teams enter their team name, 
 * personal names and login IDs in a struct of this
 * type in their bits.c file.
 */
typedef struct {
    char *teamname; /* ID1+ID2 or ID1 */
    char *name1;    /* full name of first member */
    char *id1;      /* login ID of first member */
    char *name2;    /* full name of second member (if any) */
    char *id2;      /* login ID of second member */
} team_t;

extern team_t team;

