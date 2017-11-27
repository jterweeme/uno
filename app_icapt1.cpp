/*
input capture example

D8 = ICP poort
*/

#include "misc.h"
#include "uno.h"
#include <avr/interrupt.h>
#include <stdio.h>

volatile uint16_t pulse_start = 0, pulse_width = 0;

ISR(TIMER1_CAPT_vect)
{
    if (PINB & 1<<PINB0)
    {
        pulse_start = ICR1;
        TCCR1B &= ~(1<<ICES1);
    }
    else
    {
        pulse_width = ICR1 - pulse_start;
        TCCR1B |= 1<<ICES1;
        TCNT1 = 0;
    }
}

int main()
{
    Serial s;
    s.init();
    TCCR1A = 0;
    TCCR1B = 1<<ICNC1 | 1<<ICES1 | 1<<CS10;
    TIMSK1 = 1<<ICIE1;
    sei();
    char buf[50];
    
    while (true)
    {
        snprintf(buf, 50, "%u\r\n", pulse_width);
        s.write(buf);
    }

    return 0;
}


