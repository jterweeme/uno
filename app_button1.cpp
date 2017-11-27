#include "misc.h"
#include <stdio.h>

int main()
{
    DDRC = 0;
    PORTC = 1<<PORTC0 | 1<<PORTC1 | 1<<PORTC2 | 1<<PORTC3 | 1<<PORTC4 | 1<<PORTC5;
    Serial serial;
    serial.init();
    char buf[10] = {0};

    while (true)
    {
        uint8_t x = PINC;

        switch (x)
        {
            case 0xfe:
                serial.write("ABC\r\n");
                break;
            default:
                snprintf(buf, 10, "%u\r\n", x);
                serial.write(buf);
        }
    }

    return 0;
}


