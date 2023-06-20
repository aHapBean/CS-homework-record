/* header comment
 * name:    
 * ID:      
*/

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

/* header comment 
 * name:    张祥东 Zhang Xiangdong
 * ID:      521030910206
*/
/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, x, y; 
    int _1,_2,_3,_4,_5,_6,_7,_8;
    if(M == 32 && N == 32) {            //287misses
        for(i = 0; i < N; i += 8) {     //8x8 blocking
            for(j = 0; j < M; j += 8) {
                if(i == j) {            //diagonal handle
                    for(x = i; x < i + 8; ++x) {
                        if(x != i)      //avoid race in diagonal
                            B[x - 1][x - 1] = _1;       
                        for(y = j; y < j + 8; ++y) {
                            if(x == y) {
                                _1 = A[x][y];
                            } else {
                                _2 = A[x][y];
                                B[y][x] = _2;
                            }
                        }
                        if(x == i + 7)
                            B[x][x] = _1;
                    }
                } else for(x = i; x < i + 8; ++x) {
                    for(y = j; y < j + 8; ++y) {
                        _1 = A[x][y];
                        B[y][x] = _1;
                    }
                }
            }
        }
    } else if(M == 64 && N == 64) {
        for(i = 0; i < N; i += 8) {
            for(j = 0; j < M; j += 8) {
                for(x = 0; x < 4; ++x) {    //copy top 4x8 matrix first
                    _1 = A[i + x][j];
                    _2 = A[i + x][j + 1];
                    _3 = A[i + x][j + 2];
                    _4 = A[i + x][j + 3];
                    _5 = A[i + x][j + 4];
                    _6 = A[i + x][j + 5];
                    _7 = A[i + x][j + 6];
                    _8 = A[i + x][j + 7];

                    B[j + x][i] = _1;
                    B[j + x][i + 1] = _2;
                    B[j + x][i + 2] = _3;
                    B[j + x][i + 3] = _4;
                    B[j + x][i + 4] = _5;
                    B[j + x][i + 5] = _6;
                    B[j + x][i + 6] = _7;
                    B[j + x][i + 7] = _8;
                }
                //transpose top left
                for(x = 0; x < 4; ++x) {
                    for(y = x + 1; y < 4; ++y) {
                        _1 = B[j + x][i + y];
                        B[j + x][i + y] = B[j + y][i + x];
                        B[j + y][i + x] = _1;
                    } 
                }

                //transpose top right
                for(x = 0; x < 4; ++x) {//base j,i + 4
                    for(y = x + 1; y < 4; ++y) {
                        _1 = B[j + x][i + 4 + y];
                        B[j + x][i + 4 + y] = B[j + y][i + 4 + x];
                        B[j + y][i + 4 + x] = _1;
                    }
                }

                for(x = 0; x < 4; ++x) {//design to reduce evction times between B's 8 lines
                    _1 = B[j + x][i + 4];
                    _2 = B[j + x][i + 4 + 1];
                    _3 = B[j + x][i + 4 + 2];
                    _4 = B[j + x][i + 4 + 3];
                    
                    //base i+4,y
                    B[j + x][i + 4] = A[i + 4][j + x];
                    B[j + x][i + 4 + 1] = A[i + 4 + 1][j + x];
                    B[j + x][i + 4 + 2] = A[i + 4 + 2][j + x];
                    B[j + x][i + 4 + 3] = A[i + 4 + 3][j + x];

                    //B left bottom
                    B[j + 4 + x][i] = _1;
                    B[j + 4 + x][i + 1] = _2;
                    B[j + 4 + x][i + 2] = _3;
                    B[j + 4 + x][i + 3] = _4;
                }

                //right bottom copy and transpose
                for(x = 0; x < 4; ++x) {
                    for(y = 0; y < 4; ++y) {
                        _1 = A[i + 4 + x][j + 4 + y];
                        B[j + 4 + y][i + 4 + x] = _1;
                    }
                }
            }
        }
    } else if(M == 61) {
        //N = 67 M = 61
        //first part cope 64x61 matrix
        for(i = 0; i + 8 < N; i += 8) {
            for(j = 0; j < M; ++j) {
                _1 = A[i][j];
                _2 = A[i + 1][j];
                _3 = A[i + 2][j];
                _4 = A[i + 3][j];
                _5 = A[i + 4][j];
                _6 = A[i + 5][j];
                _7 = A[i + 6][j];
                _8 = A[i + 7][j];

                B[j][i] = _1;
                B[j][i + 1] = _2;
                B[j][i + 2] = _3;
                B[j][i + 3] = _4;
                B[j][i + 4] = _5;
                B[j][i + 5] = _6;
                B[j][i + 6] = _7;
                B[j][i + 7] = _8;
            }
        }
        //second part copy the remainder area 4x61
        for(x = i; x < N; ++x) {
            for(y = 0; y < M; ++y) {
                B[y][x] = A[x][y];
            }
        }
    }  
    return ;
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