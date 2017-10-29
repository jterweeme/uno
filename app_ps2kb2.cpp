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
PS2Keyboard *g_kb;

ISR(INT0_vect)
{
    g_kb->isr();
}

inline char nibble(uint8_t n)
{
    return n <= 9 ? '0' + n : 'A' + n - 10;
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
    PS2Keyboard keyboard;
    g_kb = &keyboard;
    s.write("Keyboard Test:\r\n");

    while (true)
    {
        uint8_t x = keyboard.get_scan_code();

        if (x > 0)
        {
            s.write(nibble(x >> 4));
            s.write(nibble(x & 0xf));
            s.write("\r\n");
        }


    }

    return 0;
}




