/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
#define BLOCK_SIZE 4 //32 bytes per line, 32 lines total (1 line per set), 4 bytes per int
#define MIN(a,b) ((a) < (b) ? (a) : (b))



char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp, block_row, block_col, cur_N, cur_M;

    for(block_row = 0; block_row < N; block_row += BLOCK_SIZE*2){
        for(block_col = 0; block_col < M; block_col += BLOCK_SIZE){
            cur_N = MIN(BLOCK_SIZE*2, N-block_row);
            cur_M = MIN(BLOCK_SIZE, M-block_col);
            for (i = 0; i < cur_N; i++) {
                for (j = 0; j < cur_M; j++) {
                    tmp = A[i+block_row][j+block_col];
                    B[j+block_col][i+block_row] = tmp;
                }
            }    
        }
    }
}

unsigned int get_odd_bits(unsigned int x){
    unsigned int temp = 0;
    unsigned char digits = 0;
    while(x != 0){
        temp += x & 0x1;
        x >>= 2;
        temp <<= 1;
        digits++;
    }
    temp >>= 1;
    while(digits > 0){
        x += temp & 0x1;
        temp >>= 1;
        digits--;
    }
    return x;
}
unsigned int ceil_2pow(unsigned int x){
    unsigned int to_return = 1;
    while(x != 0){
        x >>= 1;
        to_return++;
    }
    return 1 << to_return;
}

void weird_transpose(int M, int N, int A[N][M], int B[M][N])
{
    unsigned int total_iter = ceil_2pow(M)*ceil_2pow(N);
    int idx, i, j, tmp;
    for(idx = 0; idx < total_iter; idx++){
        i = get_odd_bits(idx);
        j = get_odd_bits(idx >> 1);
        tmp = A[i][j];
        B[j][i] = tmp;
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}