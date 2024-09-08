#ifdef _WIN32

#if (_MSC_VER < 1940)
#include "stdint.h"
#else
#include <stdint.h>
#endif

#include <windows.h>

#else

#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#endif

typedef struct seed_thread_s{
    volatile uint64_t *source;
    volatile uint64_t *sink;
    int run;
#ifdef _WIN32
    HANDLE thr;
#else
    pthread_t thr;
#endif
} seed_thread;

typedef struct seed_state_s{
    volatile uint64_t nodes[3];
    seed_thread threads[3];
} seed_state;

uint64_t read_state(seed_state* state)
{
    return state->nodes[0] ^ state->nodes[1] ^ state->nodes[2];
}

int seed_modify( volatile uint64_t* in, volatile uint64_t* out )
{
    uint64_t acc = 1;                                   
    /* 
     *   newvalue : 128 unique prime numbers will be multiplied together in this accumulator
     *   The original hash function used a set of smallest prime numbers. (2, 3, 5, ...)
     *   Replacing by a personal selection of prime numbers found on the internet improved the quality of the stream.
     *   I think this is mostly because it forces to changes more bits at every addition in the collision multiplier.
     *   No other sets has been tested by the original author. Please modify this comment if you do.
     */
    uint64_t primes[128] = {2985113428254948959ULL,2133371973447287789ULL,9566925003104080411ULL,
    6083414080430737261ULL,3191658566037706763ULL,6913367267901031997ULL,3051762490125913007ULL,
    4496279044463191703ULL,2879325911889076339ULL,1685450054146532617ULL,3665492224534333199ULL,
    5457405118009752311ULL,9603674174905975319ULL,5115570615370213399ULL,2085406057152991381ULL,
    3944123521248288751ULL,7068318273059084621ULL,9757209530458778617ULL,4605875306346355289ULL,
    6173263078309968247ULL,7762991333463966857ULL,9020426977113888977ULL,7479023155365493211ULL,
    1106308364738794853ULL,6407388572493574733ULL,2575377960084637447ULL,3570804859728957167ULL,
    9744215084471400349ULL,5786617550671126231ULL,3349473746601511027ULL,4095923646587284861ULL,
    9337574612835542873ULL,3860411135543820653ULL,6490253556739218821ULL,8764900392399904901ULL,
    3339372363887497583ULL,6577701767928311429ULL,9172672799009443051ULL,2788564470716716463ULL,
    7776210437768060567ULL,9071396938031495159ULL,5357072389526583433ULL,6633238451589839561ULL,
    6234713344370965483ULL,1756087432259377561ULL,3210111471103586737ULL,2408295824875360559ULL,
    8027800137025286071ULL,1345382896972678691ULL,9079595888451204607ULL,6556450002823963531ULL,
    7301373103704944939ULL,1213337252585919101ULL,8574942160927332881ULL,8566185364500201241ULL,
    6910045784078487233ULL,2298581249625702883ULL,2596136803677419831ULL,8990581036371057601ULL,
    3046889204803159003ULL,6864701819024214703ULL,9142639517683050829ULL,3257525145999184439ULL,
    5916350576516314903ULL,1991008946812235681ULL,5221054210441722691ULL,4658950185140640559ULL,
    7616154792906075869ULL,3789735489826889479ULL,6040491638497194979ULL,4531551599015040559ULL,
    8347022247974757701ULL,5867558032219360441ULL,4346307676337591453ULL,1905346606445378533ULL,
    7219759323757378847ULL,8561163821217102983ULL,6227762971465314193ULL,1778354250598502561ULL,
    5435722899178511549ULL,7859180386742822047ULL,3610133528213982527ULL,4577787558750808879ULL,
    3974497005134013109ULL,3241288686894948329ULL,3206546850542044487ULL,2621128575668534623ULL,
    6945339563322422683ULL,2086055779082649859ULL,9237096783908108657ULL,1916568609042647407ULL,
    8362781431934621017ULL,6122543372115730003ULL,6200399818195771541ULL,1259180073496439959ULL,
    4347405785030416513ULL,4385862544209720253ULL,5836562284002027449ULL,3121678407354608831ULL,
    4319136453194505203ULL,9085838656424873461ULL,5448516463713100871ULL,4520015808866767003ULL,
    2892328915328607301ULL,3897908110284010777ULL,6252018905800065119ULL,3278710975885668139ULL,
    2623408458547857527ULL,3055342605008846819ULL,5255646576194862401ULL,5611094629836787733ULL,
    3296996236992951511ULL,3283558997806116761ULL,2823396471863459119ULL,9572308265712226037ULL,
    8290430615658140401ULL,2621246478142939039ULL,1300291691995934101ULL,1143863624227711889ULL,
    6789671898597272039ULL,1593529243588418053ULL,6573565795099723367ULL,7154224095543629129ULL,
    2355509713379653547ULL,7691022839137400143ULL,2727794217173953619ULL,8541344794240933387ULL,
    6490328854741497349ULL};
    int x = 0;
    while( x < 64 ) {
        *in = (*in << 13) | (*in >> (64 - 13));         /* source is rotated by 13 bits... */
        acc = (acc << x) | (acc >> (64 - x));           /* smooth out bit distribution in acc */
        acc *= primes[(2 * x) + (1 & *in)];               /* pick the prime[x][set?], do it 64 times (we rotate above) */

        /* 
         *   This addition increase the number of unresolved race conditions / computation cycle.
         *   Before this addition I had to wait and verify for a change in the state to avoid polling too soon.
         *   This addition instruction allowed continuous polling during most of the development and testing phases.
         *   Basic dafeguard has been reintroduced for production before the last round of testing on the iBook G4.
         */
        *out += acc ^ *in;
        x++;
    }
    *out ^= acc;
    return 0;
}

void* seed_thread_main(void* raw)
{
    seed_thread* state = (seed_thread*) raw;
    while(1)
    {
        seed_modify(state->source, state->sink);
        if(state->run == 0)
        {
            return NULL;
        }
    }
    return NULL;
}

void start_seeder(seed_state* state)
{
    int i = 0;
    state->nodes[0] = 0;
    state->nodes[1] = 0;
    state->nodes[2] = 0;
    state->threads[0].source = &(state->nodes[0]);
    state->threads[1].source = &(state->nodes[1]);
    state->threads[2].source = &(state->nodes[2]);
    state->threads[0].sink = &(state->nodes[1]);
    state->threads[1].sink = &(state->nodes[2]);
    state->threads[2].sink = &(state->nodes[0]);
    state->threads[0].run = 1;
    state->threads[1].run = 1;
    state->threads[2].run = 1;
#ifdef _WIN32
    state->threads[0].thr = CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0, (LPTHREAD_START_ROUTINE)&seed_thread_main, &(state->threads[0]), (DWORD)0, NULL);
    state->threads[1].thr = CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0, (LPTHREAD_START_ROUTINE)&seed_thread_main, &(state->threads[1]), (DWORD)0, NULL);
    state->threads[2].thr = CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0, (LPTHREAD_START_ROUTINE)&seed_thread_main, &(state->threads[2]), (DWORD)0, NULL);
#else
    pthread_create(&(state->threads[0].thr), NULL, &seed_thread_main, &(state->threads[0]));
    pthread_create(&(state->threads[1].thr), NULL, &seed_thread_main, &(state->threads[1]));
    pthread_create(&(state->threads[2].thr), NULL, &seed_thread_main, &(state->threads[2]));
#endif

    while( i < 3 )
    {
        state->nodes[i] = 0;
        state->threads[i].source = &(state->nodes[i]);
        state->threads[i].sink = &(state->nodes[(i + 1) % 3]);
        state->threads[i].run = 1;
        i++;
    }
}

void stop_seeder(seed_state* state)
{
    state->threads[0].run = 0;
    state->threads[1].run = 0;
    state->threads[2].run = 0;
#ifdef _WIN32
    WaitForSingleObject(state->threads[0].thr, 2000000000);
    WaitForSingleObject(state->threads[1].thr, 2000000000);
    WaitForSingleObject(state->threads[2].thr, 2000000000);
#else
    pthread_join(state->threads[0].thr, NULL);
    pthread_join(state->threads[1].thr, NULL);
    pthread_join(state->threads[2].thr, NULL);
#endif
}

void seedy(uint64_t* buffer, uint64_t count)
{
    int i = 0;
    uint64_t last_pick;
    uint64_t next_pick;
    seed_state state;
    start_seeder(&state);

    last_pick = read_state(&state);

    while( i < count )
    {
#ifdef _WIN32
        Sleep(1);
#else
        usleep(1000);
#endif
        next_pick = read_state(&state);
        if(next_pick != last_pick)
        {
            buffer[i] = next_pick;
            last_pick = next_pick;
            i = i + 1;
        }
    }

    stop_seeder(&state);
}