#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t lookup[] = {
    0b00000011, //0
    0b10011111, //1
    0b00100101, //2
    0b00001101, //3
    0b10011001, //4
    0b01001001, //5
    0b01000001, //6
    0b00011111, //7
    0b00000001, //8
    0b00001001  //9
};

uint8_t selector = 0;

ISR(TIMER0_OVF_vect)
{
    if (++selector > 3)
        selector = 0;

    TCNT0 = 0;
}

class Timer0
{
public:
    void init();
};

class SevenSeg
{
    uint8_t _buf[4];
public:
    void setNum(uint16_t n);
    void blit();
};

void SevenSeg::setNum(uint16_t n)
{
    _buf[0] = lookup[n / 1000 % 10];
    _buf[1] = lookup[n / 100 % 10];
    _buf[2] = lookup[n / 10 % 10];
    _buf[3] = lookup[n % 10];
}

void SevenSeg::blit()
{
    PORTC = 1 << selector;
    PORTD = _buf[selector];
}

void Timer0::init()
{
    TCNT0 = 0;
    TCCR0B = 1<<CS02;
    TIMSK0 = 1<<TOIE0;
    sei();
}

int main()
{
    DDRC = 0xff;
    DDRD = 0xff;
    Timer0 t0;
    t0.init();
    SevenSeg ss;
    ss.setNum(1234);

    while (true)
        ss.blit();

    return 0;
}


