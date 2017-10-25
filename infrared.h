#ifndef _INFRARED_H_
#define _INFRARED_H_
#include <stdint.h>

typedef
    enum {
        UNKNOWN      = -1,
        UNUSED       =  0,
        RC5,
        RC6,
        NEC,
        SONY,
        PANASONIC,
        JVC,
        SAMSUNG,
        WHYNTER,
        AIWA_RC_T501,
        LG,
        SANYO,
        MITSUBISHI,
        DISH,
        SHARP,
        DENON,
        PRONTO,
        LEGO_PF,
    }
decode_type_t;

class decode_results
{
public:
    decode_type_t          decode_type;  // UNKNOWN, NEC, SONY, RC5, ...
    unsigned int           address;      // Used by Panasonic & Sharp [16-bits]
    unsigned long          value;        // Decoded value [max 32-bits]
    int                    bits;         // Number of bits in decoded value
    volatile unsigned int  *rawbuf;      // Raw intervals in 50uS ticks
    int                    rawlen;       // Number of records in rawbuf
    int                    overflow;     // true iff IR raw code too long
};

class IRrecv
{
public:
    IRrecv(int recvpin);
    IRrecv(int recvpin, int blinkpin);
    int   decode     (decode_results *results) ;
    void  enableIRIn();
    bool  isIdle();
    void  resume();
private:
    long decodeHash(decode_results *results) ;
    int compare(unsigned int oldval, unsigned int newval);
};

class RC
{
public:
    static const uint32_t POWER = 0xe318261b;
    static const uint32_t MODE = 0x00511dbb;
    static const uint32_t MUTE = 0xee886d7f;
    static const uint32_t PLAY = 0x52a3d41f;
    static const uint32_t PREV = 0xd7e84b1b;
    static const uint32_t NEXT = 0x20fe4dbb;
    static const uint32_t EQ = 0xf076c13b;
    static const uint32_t MIN = 0xa3c8eddb;
    static const uint32_t PLUS = 0xe5cfbd7f;
    static const uint32_t N0 = 0xc101e57b;
    static const uint32_t ROTATE = 0x97483bfb;
    static const uint32_t USD = 0xf0c41643;
    static const uint32_t N1 = 0x9716be3f;
    static const uint32_t N2 = 0x3d9ae3f7;
    static const uint32_t N3 = 0x6182021b;
    static const uint32_t N4 = 0x8c22657b;
    static const uint32_t N5 = 0x488f3cbb;
    static const uint32_t N6 = 0x0449e79f;
    static const uint32_t N7 = 0x32c6fdf7;
    static const uint32_t N8 = 0x1bc0157b;
    static const uint32_t N9 = 0x3ec3fc1b;
};

#endif


