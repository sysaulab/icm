/*  Copyright 2023, All rights reserved, Sylvain Saucier
    sylvain@sysau.com
    Distributed under Affero GNU Public Licence version 3
    Other licences available upon request */

#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sr_config.h"

#define _PRNG_THREADS 3

inline uint64_t rot64(uint64_t i, int s)
{
    return (i << s) | (i >> (64 - s));
}

void* prng(void * raw)
{
    uint64_t buffer[_SSRNG_BUFLEN];
    uint64_t prng_state[4][256];
    uint64_t* cache = calloc(4*65536, sizeof(uint64_t));
    uint64_t index = 0;
    int64_t fresh = 0;
    uint16_t* indexes = (uint16_t*)&index;
    while(1){
        if(fresh < 1){
            fresh = 1099511627776;// 1TiB, way way way safe. 8PiB possible iterations
            fread( prng_state, sizeof(uint64_t), 4 * 256, stdin);
            for(int x = 0; x < 0x100; x++)
            for(int y = 0; y < 0x100; y++){
                int z = (x << 8) | (y);
                cache[0x00000+z] = rot64(prng_state[0][x], y & 0b00111111);
                cache[0x10000+z] = rot64(prng_state[1][x], y & 0b00111111);
                cache[0x20000+z] = rot64(prng_state[2][x], y & 0b00111111);
                cache[0x30000+z] = rot64(prng_state[3][x], y & 0b00111111);}}
        for( uint64_t y = 0; y < _SSRNG_BUFLEN; y++ ){
            buffer[y] = (cache[indexes[0]] ^ cache[indexes[1]]) ^ (cache[indexes[2]] ^ cache[indexes[3]]);
            index = index + 7776210437768060567ULL;
            fresh--;}
        fwrite(buffer, sizeof(uint64_t), _SSRNG_BUFLEN, stdout);}
    return NULL;
}

int main(int argc, char** argv)
{
    pthread_t threads[_PRNG_THREADS];

    for (int x = 0; x < _PRNG_THREADS; x++)
        pthread_create(&threads[x], NULL, prng, NULL);

    for (int x = 0; x < _PRNG_THREADS; x++)
        pthread_join(threads[x], NULL);

    return EXIT_SUCCESS;
}

