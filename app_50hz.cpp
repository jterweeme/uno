#include <avr/interrupt.h>

#define F_CPU 16000000UL
#include <util/delay.h>

ISR(TIMER1_COMPA_vect)
{
    PORTD ^= 1<<PD5;
}

int main()
{
    DDRD |= 1<<PD6 | 1<<PD3 | 1<<PD2 | 1<<PD5; // LP, FLM, CP, M
    DDRC |= 1<<PC0 | 1<<PC1 | 1<<PC2 | 1<<PC3; // Data
    _delay_ms(100);
    

    while (true)
    {
        _delay_ms(10);
        PORTD |= 1<<PD3 | 1<<PD2;
        _delay_us(1);
        PORTD &= ~(1<<PD2);     // CP
        _delay_us(2);

        for (uint8_t i = 0; i < 128; i++)
        {
            for (uint8_t i = 0; i < 64; i++)
            {
                _delay_us(1);
                PORTC |= 1<<0 | 1<<1;
                PORTC &= ~(1<<2 | 1<<3);
                _delay_us(1);
                PORTD |= 1<<PD2;
                _delay_us(1);
                PORTD &= ~(1<<PD2);
                _delay_us(1);
            }

            _delay_us(1);
            PORTD |= 1<<PD6;
            _delay_us(1);
            PORTD |= 1<<PD2;    // CP
            _delay_us(1);
            PORTD &= ~(1<<PD2); // CP
            _delay_us(1);
            PORTD &= ~(1<<PD6);
            _delay_us(1);
            PORTD |= 1<<PD2;    // CP
            _delay_us(1);
            PORTD &= ~(1<<PD2); // CP
            _delay_us(1);
        }

        PORTD &= ~(1<<PD3); // FLM
        _delay_us(1);
        PORTD |= 1<<PD2;    // CP
        _delay_us(1);
        PORTD &= ~(1<<PD2); // CP
        _delay_us(1);
    }

    return 0;
}


