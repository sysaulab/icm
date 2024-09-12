#include "seedy.h"



void qxo32_init(QXO32* q, void* f)
{
    int x = 0;
    int y = 0;
    q->iter = 1234567907;
    q->step = 0;
    q->feeder = f;
}

uint32_t qxo32_at(QXO32* q, uint32_t i)
{
    uint32_t pos32;
    uint16_t* pos16 = (uint16_t*)&pos32;
    uint8_t* pos8   = (uint8_t*) &pos32;
    uint16_t* mid = (uint16_t*)&pos8[1];
    pos32 = i * q->iter;

    return  q->pool[0][pos16[0]] ^
            q->pool[0][*mid]>>3  ^  /* This deviation from QXO64 makes it pass practrand to 32GB. */
            q->pool[1][*mid]<<5  ^  /* The goal is to offer a fast supplement to the MT on win32. */
            q->pool[1][pos16[1]] ;
}

uint32_t qxo32_next(QXO32* q)
{
    uint32_t next, pos32;
    uint16_t* pos16 = (uint16_t*)&pos32;

    if( q->step == 0 )
    {
        ((void(*)(uint8_t* b, uint32_t n))q->feeder)((uint8_t*)&q->pool, sizeof(q->pool));
        q->step++;
    }
    next = qxo32_at(q, q->step);
    q->step++;

    return next;
}

