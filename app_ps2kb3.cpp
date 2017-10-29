#include "misc.h"
#include <avr/interrupt.h>

class PS2Keyboard
{
private:
    MyBitset _bitset;
    CircBuf _buf;
    uint32_t _prev_ticks = 0;
public:
    PS2Keyboard();
    uint8_t get_scan_code() { return _buf.get(); }
    void isr();
};

static uint32_t overflows0 = 0;

ISR(TIMER0_OVF_vect)
{
    overflows0++;
}

PS2Keyboard *g_kb;
Serial *g_serial;

ISR(INT0_vect)
{
    g_kb->isr();
}

void PS2Keyboard::isr()
{
    uint8_t val = PINB & 1<<0;

    if (overflows0 - _prev_ticks > 250)
        _bitset.reset();

    _prev_ticks = overflows0;
    _bitset.addBit(val);

    if (_bitset.count() == 11)
    {
        _buf.push(_bitset.incoming());
        _bitset.reset();
    }
}

inline char nibble(uint8_t n)
{
    return n <= 9 ? '0' + n : 'A' + n - 10;
}

PS2Keyboard::PS2Keyboard()
{
    DDRD &= ~(1<<DDD3 | 1<<DDD2);
    PORTD |= 1<<PORTD3 | 1<<PORTD2;
    DDRB &= ~(1<<DDB0);
    PORTB |= 1<<PORTB0;
    EICRA |= 1<<ISC00 | 1<<ISC01;   // external interrupt 0 falling edge?
    EIMSK |= 1<<INT0;               // enable interrupt0
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




