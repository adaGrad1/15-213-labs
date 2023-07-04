#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "cachelab.h"

struct node{struct node* next; unsigned int tag;};

unsigned char access_cache(unsigned int address, struct node* cache[], char s, unsigned int E, char b){
//returns 0 if hit, 1 if miss, 2 if miss evict
    unsigned int ones_mask = (1 << s) - 1;
    unsigned int set_idx = (address >> b) & ones_mask;
    unsigned int tag = (address >> (b+s));
    //seek to see if we can find tag in the linked list with set_idx
    //this is pretty slow but whatever i dont feel like figuring out hash tables in C
    unsigned int set_len = 0;
    struct node* next_node = cache[set_idx];
    struct node* cur_node = NULL;
    struct node* prev_node = NULL;

    while(next_node != NULL){
        set_len++;
        prev_node = cur_node;
        cur_node = next_node;
        next_node = cur_node->next;
        if(cur_node->tag == tag){
            //move node with accessed value to front of cache
            if(prev_node != NULL){
                prev_node->next = cur_node->next;
                cur_node->next = cache[set_idx];
                cache[set_idx] = cur_node;
            }
            return 0; //hit!
        }
    }
    //miss -- add a new node to the start
    struct node* new_node = malloc(sizeof(struct node));
    new_node->tag = tag;
    new_node->next = cache[set_idx];
    cache[set_idx] = new_node;
    if(set_len == E){
        //no space -- must evict!
        if(set_len > 1) prev_node->next = NULL;
        else new_node->next = NULL; //handle edge case of size 1 line
        return 2; //miss evict!
    }
    return 1;
}

char* code_to_string(char code){
    switch(code){
        case 0:
        return "hit";
        case 1:
        return "miss";
        case 2:
        return "miss eviction";
    }
    return "invalid";
}

int main(int argc, char *argv[])
{

    char verbose = 0;
    char s = -1;
    unsigned int E = -1;
    char b = -1;
    char *trace_file_path = NULL;

    for (int i = 1; i < argc; i++) {
        char* cur_str = argv[i];
        assert(cur_str[0] == '-');
        switch(cur_str[1]){
            case 'v':
                verbose = 1;
                break;
            case 's':
                i++;
                s = atoi(argv[i]);
                break;
            case 'E':
                i++;
                E = atoi(argv[i]);
                break;
            case 'b':
                i++;
                b = atoi(argv[i]);
                break;
            case 't':
                i++;
                trace_file_path = argv[i];
                break;
        }
    }

    assert(s >= 1);
    assert(E >= 1);
    assert(b >= 1);
    assert(trace_file_path != NULL);

    struct node* cache[1 << s];
    for(int i = 0; i < (1 << s); i++){
        cache[i] = NULL;
    }



    FILE* f = fopen (trace_file_path, "rb");

    int bufferLength = 255;
    char line[bufferLength]; /* not ISO 90 compatible */

    int results[3] = {0,0,0};
    while (fgets(line, bufferLength, f)) {
        if(line[0] == 'I') continue; //ignore instructions
        char instr_type = 0;
        unsigned int address = 0;
        unsigned int read_size = 0; 
        sscanf(line, " %c %x,%x", &instr_type, &address, &read_size);            
        char res = access_cache(address, cache, s, E, b);
        results[(int)res]++;
        char* res_str = code_to_string(res);
        char* res_str_2 = "";
        if(instr_type == 'M'){
            res = access_cache(address, cache, s, E, b);
            results[(int)res]++;
            res_str_2 = code_to_string(res);
        }
        if(verbose) printf("%c %x,%x %s %s\n", instr_type, address, read_size, res_str, res_str_2);
    }
    printSummary(results[0], results[1]+results[2], results[2]);
    return 0;
}
