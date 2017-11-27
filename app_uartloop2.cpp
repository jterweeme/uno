// Reads from serialport, writes back changing uppercase to lowercase and
// vice versa using interrupt method

#include "misc.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>

Serial *g_serial;

ISR(USART_RX_vect)
{
    g_serial->write(CType::convert(UDR0));
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
        sleep_mode();

    return 0;
}


