#include "misc.h"
#include <avr/interrupt.h>

Serial *g_serial;

ISR(PCINT0_vect)
{
    g_serial->write("K0\r\n");
}

ISR(PCINT1_vect)
{
    g_serial->write("K1\r\n");
}

int main()
{
    Serial serial;
    g_serial = &serial;
    serial.init();
    DDRB &= ~(1<<DDB0);
    DDRC &= ~(1<<DDC0);
    PORTB |= 1<<PORTB0;
    PORTC |= 1<<PORTC0;
    PCICR |= 1<<PCIE0 | 1<<PCIE1;
    PCMSK0 |= 1<<PCINT0;
    PCMSK1 |= 1<<PCINT8;
    sei();
    
    while (true)
        ;

    return 0;
}


