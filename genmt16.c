#include "mt16.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    mt_t seed;
    arc4random_buf(&seed, sizeof(seed));
    MT16 prng;
    mt16_init(&prng, seed);
    
    while (1)
    {
        mt_t buf[1024];

        for ( int x = 0; x < 1024; x++)
            buf[x] = mt16_next(&prng);

        fwrite(buf, sizeof(buf), 1, stdout);
    }
    return 0;
}