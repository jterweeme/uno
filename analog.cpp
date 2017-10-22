#include "analog.h"

void Analog::init()
{
    ADMUX = 1<<REFS0;
    ADCSRA = 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0 | 1<<ADEN;
}

uint16_t Analog::read(uint8_t input)
{
    ADMUX &= ~(1<<MUX0 | 1<<MUX1 | 1<<MUX2 | 1<<MUX3); // clear mux bits
    ADMUX |= input; // select input port
    ADCSRA |= 1<<ADSC;

    while (ADCSRA & 1<<ADSC)
        ;

    return ADCL | (uint16_t)ADCH << 8;
}

uint8_t DFKeypad::read()
{
    uint16_t x = _adc->read(Analog::ADC0);

    if (x < 100)
        return RIGHT;

    if (x < 250)
        return UP;

    if (x < 400)
        return DOWN;

    if (x < 550)
        return LEFT;

    if (x < 800)
        return SELECT;

    return 0;
}

TSPoint TouchScreen::getPoint(Analog &analog)
{
    int samples[SAMPLES];
    uint8_t valid = 1;
    DDRC &= ~(1<<3);
    DDRB &= ~(1<<1);
    DDRB |= 1<<0;
    DDRC |= 1<<2;
    PORTB |= 1<<0;
    PORTC &= ~(1<<2);

    for (uint8_t i = 0; i < SAMPLES; i++)
        samples[i] = analog.read(Analog::ADC3);

    if (samples[0] - samples[1] < -4 || samples[0] - samples[1] > 4)
        valid = 0;
    else
        samples[1] = (samples[0] + samples[1]) >> 1; // average 2 samples

    //uint16_t x = 1023 - samples[2/2];
    uint16_t x = samples[2/2] % 1024;
    x -= 200;
    x /= 2.9;
    DDRB &= ~(1<<0);
    DDRC &= ~(1<<2);
    DDRC |= 1<<3;
    DDRB |= 1<<1;
    PORTB &= ~(1<<1);
    PORTC |= 1<<3;

    for (uint8_t i = 0; i < SAMPLES; i++)
        samples[i] = analog.read(Analog::ADC2);

    if (samples[0] - samples[1] < -4 || samples[0] - samples[1] > 4)
        valid = 0;
    else
        samples[1] = (samples[0] + samples[1]) >> 1;

    uint16_t y = 1023 - samples[SAMPLES/2];
    y -= 90;
    y /= 2.5;
    DDRB |= 1<<0;       //xp
    DDRC &= ~(1<<3);    //yp
    PORTB &= ~(1<<0);   //xp
    PORTB |= 1<<1;      //ym
    uint16_t z1 = analog.read(Analog::ADC2);
    uint16_t z2 = analog.read(Analog::ADC3);
    uint16_t z = 1023 - (z2 - z1);

    if (!valid)
        z = 0;

    return TSPoint(x, y, z);
}


