#include <avr/interrupt.h>
#include "misc.h"

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <util/delay.h>
#include "infrared.h"

static inline char nibble(uint8_t n)
{
    return n <= 9 ? '0' + n : 'A' + n - 10;
}

int main()
{
    int RECV_PIN = 10;
    IRrecv irrecv(RECV_PIN);
    decode_results results;
    Serial serial;
    serial.init();
    serial.write("Enabling IRin\r\n");
    irrecv.enableIRIn();
    serial.write("Enabled IRin\r\n");

    while (true)
    {
        if (irrecv.decode(&results))
        {
            for (int8_t i = 7; i >= 0; i--)
                serial.write(nibble(results.value >> (i << 2) & 0xf));

            serial.write("\r\n");
            irrecv.resume();
        }

        _delay_ms(100);
    }

    return 0;
}



