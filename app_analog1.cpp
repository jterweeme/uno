/*
reads out analog port 2 and prints the value
to UART
*/

#include "analog.h"
#include <stdio.h>
#include "misc.h"

int main()
{
    Analog adc;
    adc.init();
    Serial s;
    s.init();

    while (true)
    {
        uint16_t x = adc.read(Analog::ADC2);

        char buf[100];
        snprintf(buf, 100, "%u\r\n", x);
        s.write(buf);
    }

    return 0;
}


