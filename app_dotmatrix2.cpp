/*
D4 = D4 = PORTD4
D5 = D5 = PORTD5
D6 = D6 = PORTD6
D7 = D7 = PORTD7
RS = D8 = PORTB0
E = D9 = PORTB1
*/

#include <avr/interrupt.h>

#define F_CPU 16000000L
#include <util/delay.h>
#include <stdio.h>
#include "lcd1602.h"

int main()
{
    LCD1602 lcd;
    lcd.init();
    lcd.begin(16,2);
    lcd.write("Apollo");

    while (true)
    {
    }

    return 0;
}



