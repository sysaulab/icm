/*  Copyright 2023, All rights reserved, Sylvain Saucier
    sylvain@sysau.com
    Distributed under Affero GNU Public Licence version 3
    Other licences available upon request */

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "ftime.h"
#include "config.h"

double start = 0;
uint64_t bytes_written = 0;

void* live(void* unused)
{
    printf("Distributed under Affero GNU Public Licence version 3\n\n");
    while(1)
    {
        usleep(1000000/20);
        fprintf(stderr, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
        fprintf(stderr, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");

        double speed = bytes_written / (ftime() - start);
        if(speed > 1000000000000)
            fprintf(stderr, "%0.3f tb/s (%llu)   ", speed / 1000000000000, bytes_written);
        else if(speed > 1000000000)
            fprintf(stderr, "%0.3f gb/s (%llu)   ", speed / 1000000000, bytes_written);
        else if(speed > 1000000)
            fprintf(stderr, "%0.2f mb/s (%llu)   ", speed / 1000000, bytes_written);
        else if(speed > 1000)
            fprintf(stderr, "%0.1f kb/s (%llu)   ", speed / 1000, bytes_written);
        else
            fprintf(stderr, "%0.0f b/s (%llu)    ", speed, bytes_written);
    }
}

int main(int argc, char** argv)
{
    uint64_t bytes_readed = 0;
    char buffer[_SSRNG_BUFSIZE];
    start = ftime();
    pthread_t thr;
    pthread_create(&thr, NULL, &live, NULL);
    while(1)
    {   
        uint64_t buf_received = fread(&buffer, sizeof(char), _SSRNG_BUFSIZE, stdin);
        if(!buf_received)
            break;
        bytes_readed += buf_received;
        bytes_written += fwrite(&buffer, sizeof(char), buf_received, stdout);
    }
    return EXIT_SUCCESS;
}