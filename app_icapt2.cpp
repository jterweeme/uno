/*
input capture example

D8 = ICP poort
*/

#include "misc.h"
#include "uno.h"
#include <avr/interrupt.h>
#include <stdio.h>

volatile uint32_t pulse_start = 0, pulse_width = 0;
volatile uint32_t ticks = 0;

ISR(PCINT1_vect)
{
    ticks += TCNT0;
    TCNT0 = 0;

    if (PINC & 1<<PINC0)
        pulse_start = ticks;
    else
        pulse_width = ticks - pulse_start;
}

ISR(TIMER0_OVF_vect)
{
    ticks += 0xff;
}

int main()
{
    Serial s;
    s.init();
    TCCR0A = 0;
    TCCR0B = 1<<CS01 | 1<<CS00;
    TIMSK0 = 1<<TOIE0;
    PCICR |= 1<<PCIE0 | 1<<PCIE1;
    PCMSK1 |= 1<<PCINT8;
    sei();
    char buf[50];
    
    while (true)
    {
        snprintf(buf, 50, "%u\r\n", pulse_width);
        s.write(buf);
    }

    return 0;
}


