#include "seedy.h"

int isPrime64(uint64_t n) 
{ uint64_t i=5;
    if ( n <= 1 ) return 1; 
    if ( n <= 3 ) return 0; 
    if ( n % 2 == 0 || n % 3 == 0 ) return 0; 
    while ( i * i <= n ){
        if ( n % i == 0 || n % ( i + 2 ) == 0 ) 
           return 0; 
        i = i + 6;
    }
    return 1; 
} 

uint64_t prime64(uint64_t N)
{
    if (N <= 1) return 2;
    uint64_t prime = N;
    int found = 0;
     while (found == 0)
     {
        prime++;
        if (isPrime64(prime))
            found = 1;
    }
    return prime;
}

