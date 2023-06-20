/* 
 * name:    
 * ID:      
*/
#include "cachelab.h"
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#define NAME_MAXLENGTH 64
#define ull unsigned long long
int hit = 0,miss = 0,evict = 0; //record of hit,miss,evict times

int verbose_flag = 0;           //-v parameter
int time = 0;                   //time stamp to record the time for LRU
struct cache_line {             //cache line struct 
    int valid_bit;
    ull tag;
    int time_stamp;
};
struct cache_line **cache;

int s,b,E,S,B;
int min(int a,int b) {
    if(a > b) return b;
    return a;
}

void cache_access(ull address_but_b_bits,int modify_flag) {
    // (tag + s) bits which remove lower b_bits
    // because we only need to calculate the times of hit/miss/evict,so simply remove lower b bits is ok. 
    
    ull set_index = address_but_b_bits - ((address_but_b_bits >> s) << s);  //get the set number
    ull tag = address_but_b_bits >> s;
    //hit
    for(int i = 0; i < E; ++i) {
        if(cache[set_index][i].valid_bit == 1 && cache[set_index][i].tag == tag) {
            hit += 1;
            cache[set_index][i].time_stamp = time;
            if(modify_flag == 1) hit += 1;
            if(verbose_flag) {
                if(modify_flag) 
                    printf(" hit hit\n");
                else 
                    printf(" hit\n");
            }
            return ;
        }
    }
    //miss and no evict
    for(int i = 0; i < E; ++i) {
        if(cache[set_index][i].valid_bit == 0) {
            cache[set_index][i].tag = tag;
            cache[set_index][i].time_stamp = time;
            cache[set_index][i].valid_bit = 1;
            miss += 1;
            if(modify_flag == 1) hit += 1;
            if(verbose_flag) {
                if(modify_flag) 
                    printf(" miss hit\n");
                else 
                    printf(" miss\n");
            }
            return ;
        }
    }
    //miss and evict
    int mn_time_stamp = 0x7f7f7f7f;     //record the minimum time stamp in the cache | initial value: inf 
    for(int i = 0; i < E; ++i) {
        mn_time_stamp = min(mn_time_stamp, cache[set_index][i].time_stamp);
    }
    for(int i = 0; i < E; ++i) {
        if(mn_time_stamp == cache[set_index][i].time_stamp) {
            evict += 1;
            miss += 1;
            if(modify_flag == 1) hit += 1;
            cache[set_index][i].tag = tag;
            cache[set_index][i].time_stamp = time;
            cache[set_index][i].valid_bit = 1;
            if(verbose_flag) {
                if(modify_flag) 
                    printf(" miss eviction hit\n");
                else 
                    printf(" miss eviction\n");
            }
        }
    }
}
int main(int argc,char **argv)
{
    int opt;
    char *name;
    name = malloc(NAME_MAXLENGTH * sizeof(char));
    while(-1 != (opt = getopt(argc, argv, "vs:E:b:t:"))) {
        switch(opt) {       //parse the input with form ./csim -s x -E x -b x -t xxtrace
            case 'v':
                verbose_flag = 1;
                break;
            case 's':       //set number
                s = atoi(optarg);
                break;
            case 'E':       //set associativity
                E = atoi(optarg);
                break;
            case 'b':       //block offset
                b = atoi(optarg);
                break;
            case 't':
                strcpy(name, optarg);
                break;
        }    
    }
    //get 2^s 2^b
    S = 1,B = 1;
    for(int i = 0; i < s; ++i) S *= 2;
    for(int i = 0; i < b; ++i) B *= 2;
    //malloc cache[S][E]
    cache = (struct cache_line **) malloc (sizeof(struct cache_line *) * S);//free !!

    for(int i = 0; i < S; ++i) {
        cache[i] = (struct cache_line *) malloc (sizeof(struct cache_line) * E);
        for(int j = 0; j < E; ++j) {
            cache[i][j].valid_bit = 0;  //0 symbolizes invalid
            cache[i][j].time_stamp = 0;
            cache[i][j].tag = 0;
        }
    }

    FILE *pFile = fopen(name,"r");
    if(pFile == NULL) {
        printf("open error %d\n",E);
    }
    char identifier;
    ull address;
    int size;
    while(fscanf(pFile, " %c %llx,%d", &identifier, &address, &size) > 0) {
        if(identifier == 'I') continue;
        if(verbose_flag)
            printf("%c %llx,%d",identifier,address,size);
        
        ull start_address = address;
        ull start_E = start_address >> b;   //remove lower b bits
        if(identifier == 'L') {             //load
            time += 1;
            cache_access(start_E,0);        
        } else if(identifier == 'S') {      //store
            time += 1;
            cache_access(start_E,0);
        } else if(identifier == 'M') {      //modify
            time += 1;
            cache_access(start_E,1);
        }
    }
    printSummary(hit, miss, evict);
    //free malloc space
    for(int i = 0; i < S; ++i) {
        free(cache[i]);
    }
    free(cache);
    free(name);
    return 0;
}
