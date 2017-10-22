/*
Not very useful
*/

#include <avr/io.h>

#define F_CPU 16000000
#include <util/delay.h>

int melody[] = {  1912, 2028, 2550,  1912, 2028,   3038,  0,  1912,
    3830, 2550, 2272, 1912 };
int beats[]  = { 16, 16, 16,  8,  8,  16, 32, 16, 16, 16, 8, 8 };
int MAX_COUNT = sizeof(melody) / 2;

void mydelay(uint16_t x)
{
    for (uint16_t i = 0; i < x >> 3; i++)
        _delay_us(8);
}

long tempo = 10000;
int pause = 1000;
int rest_count = 100;
int tone_ = 0;
int beat = 0;
long duration = 0;

void playTone()
{
    long elapsed_time = 0;
    
    if (tone_ > 0)
    {
        while (elapsed_time < duration)
        {
            PORTB |= 1<<3;
            mydelay(tone_ / 2);
            PORTB &= ~(1<<3);
            mydelay(tone_ / 2);
            elapsed_time += (tone_);
        }
    }
    else
    {
        for (int j = 0; j < rest_count; j++)
        {
            mydelay(duration);
        }
    }
}

int main()
{
    DDRB |= 1<<3;

    while (true)
    {
        for (int i = 0; i < MAX_COUNT; i++)
        {
            tone_ = melody[i];
            beat = beats[i];
            duration = beat * tempo;
            playTone();
            mydelay(beat);
        }
        
    }

    return 0;
}


