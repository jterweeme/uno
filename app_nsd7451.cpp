/*
lcd module
*/

#include <avr/io.h>

int main()
{
    DDRD |= 1<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6 | 1<<7;
    PORTD |= 1<<4;

    while (true)
        ;

    return 0;
}


