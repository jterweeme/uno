#include <avr/interrupt.h>
#include "uno.h"

#define F_CPU 16000000UL
#include <util/delay.h>

static constexpr uint16_t PPM_PulseLen = 300;
static constexpr uint8_t chanel_number = 8;
static constexpr uint16_t PPM_FrLen = 22500;

uint16_t ppm[chanel_number] = {1500,1500,1500,1500,1500,1500,1500,1500};

volatile uint32_t pulse_start[chanel_number] = {0};
volatile bool prev[chanel_number] = {0};
volatile uint32_t ticks = 0;

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

ISR(PCINT1_vect)
{
    cli();
    ticks += TCNT0;
    TCNT0 = 0;
    
    for (uint8_t i = 0; i < 3; i++)
    {
        if ((PINC & 1<<i) != prev[i])
        {
            if (PINC & 1<<i)
                pulse_start[i] = ticks;
            else
                ppm[i] = (ticks - pulse_start[i]) * 5 - 500;
        }

        prev[i] = PINC & 1<<i;
    }
    sei();
}

ISR(TIMER0_OVF_vect)
{
    ticks += 0xff;
}

int main()
{
    DDRC = 0;
    DDRB |= 1<<ddb2;
    *pportb &= ~(1<<pb2);
    cli();
    *ptccr1a = 0;
    *pocr1a = 100;
    TCCR0A = 0;
    TCCR0B = 1<<CS01 | 1<<CS00;
    TIMSK0 = 1<<TOIE0;
    PCICR |= 1<<PCIE1;
    PCMSK1 |= 1<<PCINT8 | 1<<PCINT9 | 1<<PCINT10 | 1<<PCINT11 | 1<<PCINT12 | 1<<PCINT13;
    TCCR1B = 1<<WGM12 | 1<<CS11;
    *ptimsk1 |= 1<<OCIE1A;
    sei();

    while (true)
        ;

    return 0;
}


