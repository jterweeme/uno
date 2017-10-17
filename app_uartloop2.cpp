// Reads from serialport, writes back changing uppercase to lowercase and
// vice versa using interrupt method

#include "misc.h"
#include <avr/interrupt.h>

Serial *g_serial;

bool isUpper(char c)
{
    return c >= 65 && c <= 90;
}

bool isLower(char c)
{
    return c >= 97 && c <= 122;
}

char convert(char c)
{
    if (isUpper(c))
        return c + 32;

    if (isLower(c))
        return c - 32;

    return c;
}

ISR(USART_RX_vect)
{
    g_serial->myPutc(convert(UDR0));
}

int main()
{
    Serial serial;
    serial.init();
    serial.enableRead();
    serial.enableReadInterrupt();
    g_serial = &serial;
    sei();

    while (true)
        ;

    return 0;
}

