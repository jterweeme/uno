#include "misc.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define F_CPU 16000000UL
#include <util/delay.h>
#include "keyboard.h"

uint32_t overflows0 = 0;

ISR(TIMER0_OVF_vect)
{
    overflows0++;
}

Serial *g_serial;
USKeyboard *g_kb;


ISR(INT0_vect)
{
    g_kb->isr();
}

int main()
{
    sei();
    TCCR0A = 0;
    TCCR0B = 1<<CS01 | 1<<CS00;     // clk/64
    TCNT0 = 0;
    TIMSK0 = 1<<TOIE0;              // timer0 overflow generates interrupt
    Serial s;
    s.init();
    g_serial = &s;
    USKeyboard keyboard;
    g_kb = &keyboard;
    s.write("Keyboard Test:\r\n");

    while (true)
    {
        if (keyboard.available())
            s.write(keyboard.read());
    }

    return 0;
}




