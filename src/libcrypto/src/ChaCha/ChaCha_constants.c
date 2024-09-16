#include "include/ChaCha_constants.h"
#include "include/ChaCha.h"

const uint8_t ChaChaConstant[16] = {
    'e',
    'x',
    'p',
    'a',
    'n',
    'd',
    ' ',
    '3',
    '2',
    '-',
    'b',
    'y',
    't',
    'e',
    ' ',
    'k',
};

#if defined(TESTING) || defined(DEBUGGER)

const uint8_t ChaCha_Test_Key_256[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
};

const uint8_t ChaCha_Test_Nonce[] = {
    0x00,
    0x00,
    0x00,
    0x09,
    0x00,
    0x00,
    0x00,
    0x4a,
    0x00,
    0x00,
    0x00,
    0x00,
};

#endif
