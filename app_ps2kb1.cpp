#include "misc.h"
#include <avr/interrupt.h>

ISR(INT0_vect)
{
    static uint8_t bitcount = 0;
    static uint8_t incoming = 0;
    static uint32_t prev_ms = 0;
    uint32_t now_ms;
    uint8_t n;
    uint8_t val = PIND;
    //now_ms = millis();
}

class PS2Keyboard
{
public:
    void init();
};

void PS2Keyboard::init()
{
    DDRD &= ~(1<<3);
    DDRB &= ~(1<<0);
    PORTD |= 1<<3;
    PORTB |= 1<<0;
}

int main()
{
    sei();
    PS2Keyboard kb;
    kb.init();

    while (true)
    {
    }

    return 0;
}



