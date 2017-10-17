/*
D6 = D6 = PORTD6
D7 = D7 = PORTD7
RS = D8 = PORTB0
E = D9 = PORTB1

untested
*/

#include <avr/interrupt.h>

#define F_CPU 16000000L
#include <util/delay.h>

class LCD
{
public:
    static const uint8_t CLEAR = 1;
    void init();
    void write(char c);
    void command(uint8_t cmd);
};

void LCD::init()
{
    DDRB = 0xff;
    DDRD = 0xff;
    _delay_ms(15);
    PORTD = 32|24;
    PORTD = 32|24;
    PORTD = 32|24;
    PORTD = 32|24;
    PORTD = 8|4;
    PORTD = 4|2;
    PORTD = 1;
}

void LCD::write(char c)
{
    PORTB |= 1<<0;
    PORTB |= 1<<1;
    PORTD = c;
    PORTB &= ~(1<<7);
    _delay_us(1520);
}

void LCD::command(uint8_t cmd)
{
    PORTB &= ~(1<<0);
    PORTB |= 1<<1;
    PORTD = cmd;
    PORTB &= ~(1<<1);
    _delay_us(1520);
}

int main()
{
    LCD lcd;
    lcd.init();
    lcd.command(1);
    lcd.command(0x0c);
    lcd.write('a');

    while (true)
        ;

    return 0;
}



