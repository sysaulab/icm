#include "tools.h"
#include "../libseedy/libseedy32.h"
#include "algos/cc2032.h"
#include "algos/qx32.h"

CC2032 CCprng;

void mt_seeder(uint8_t* b, size_t n)
{
    cc2032_fill(&CCprng,b,n);
}

int main(int argc, char** argv)
{
    QX32 QXprng;
    uint8_t buf[10240];

    cc2032_init(&CCprng, seedy32);
    qx32_init(&QXprng, (void*)&mt_seeder);

    while (1)
    {
        qx32_fill(&QXprng, buf, sizeof(buf));
        fwrite(buf, sizeof(buf), 1, stdout);
    }
    return 0;
}


