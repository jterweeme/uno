#include <avr/io.h>

int main()
{
    DDRB |= 1<<3;
    OCR2A = 100;
    TCCR2A |= 1<<WGM21 | 1<<COM2A0;
    TCCR2B |= 1<<CS21 | 1<<CS20;

    while (true)
        ;

    return 0;
}


