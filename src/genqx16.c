#include "seedy.h"

int main(int argc, char** argv)
{
    QX16 prng;
    qx16_init(&prng, parseinputgen(argc, argv));
    
    while (1)
    {
        uint16_t buf[1024];

        for ( int x = 0; x < 1024; x++)
            buf[x] = qx16_next(&prng);

        fwrite(buf, sizeof(buf), 1, stdout);
    }
    return 0;
}

