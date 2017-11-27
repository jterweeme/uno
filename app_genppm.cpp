#include <avr/interrupt.h>
#include "uno.h"

#define F_CPU 16000000UL
#include <util/delay.h>

static constexpr uint16_t PPM_PulseLen = 300;
static constexpr uint8_t chanel_number = 8;
static constexpr uint16_t PPM_FrLen = 22500;

uint16_t ppm[chanel_number] = {1400,1100,1200,1300,1500,1600,1700,1800};

ISR(TIMER1_COMPA_vect)
{
    static bool state = true;
    *ptcnt1 = 0;

    if (state)
    {
        PORTB |= 1<<pb2;
        OCR1A = PPM_PulseLen<<1;
        state = false;
    }
    else
    {
        static uint8_t cur_chan_numb = 0;
        static uint16_t calc_rest = 0;
        PORTB &= ~(1<<PORTB2);
        state = true;

        if (cur_chan_numb >= chanel_number)
        {
            cur_chan_numb = 0;
            calc_rest += PPM_PulseLen;
            OCR1A = (PPM_FrLen - calc_rest) << 1;
            calc_rest = 0;
        }
        else
        {
            OCR1A = (ppm[cur_chan_numb] - PPM_PulseLen) << 1;
            calc_rest += ppm[cur_chan_numb++];
        }
    }
}

int main()
{
    DDRB |= 1<<ddb2;
    *pportb &= ~(1<<pb2);
    cli();
    *ptccr1a = 0;
    *pocr1a = 100;
    TCCR1B = 1<<WGM12 | 1<<CS11;
    *ptimsk1 |= 1<<OCIE1A;
    sei();
    uint16_t val[chanel_number] = {1,1,1,1,1,1,1,1};

    while (true)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            ppm[i] += val[i];

            if (ppm[i] >= 1900)
                val[i] = -2;

            if (ppm[i] <= 1100)
                val[i] = 2;
        }

        _delay_ms(10);
    }

    return 0;
}


