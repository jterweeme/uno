#include "keyboard.h"


PS2Keyboard::PS2Keyboard()
{
    DDRD &= ~(1<<DDD3 | 1<<DDD2);
    PORTD |= 1<<PORTD3 | 1<<PORTD2;
    DDRB &= ~(1<<DDB0);
    PORTB |= 1<<PORTB0;
}

void PS2Keyboard::isr()
{
    uint8_t val = PINB & 1<<0;

    if (_timeTicks - _prevTicks > 10)
        _bitset.reset();

    _prevTicks = _timeTicks;
    _bitset.addBit(val);

    if (_bitset.count() == 11)
    {
        _buf.push(_bitset.incoming());
        _bitset.reset();
    }
}



