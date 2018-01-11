#include "lcd1602.h"
#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <util/delay.h>

void LCD1602::init()
{
    DDRB |= 1<<0 | 1<<1 | 1<<2;
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    begin(16, 1);
    backlight(true);
}

void LCD1602::begin(uint8_t cols, uint8_t lines, uint8_t dotsize)
{
    if (lines > 1)
        _displayfunction |= LCD_2LINE;

    _numlines = lines;
    _currline = 0;

    if ((dotsize != 0) && (lines == 1))
        _displayfunction |= LCD_5x10DOTS;

    _delay_ms(5);
    PORTB &= ~(1<<0);   // RS
    PORTB &= ~(1<<1);
    write4bits(0x03);
    _delay_us(4500); // wait min 4.1ms
    write4bits(0x03);   // second try
    _delay_us(4500); // wait min 4.1ms
    write4bits(0x03);   // third go!
    _delay_us(150);
    write4bits(0x02);
    command(LCD_FUNCTIONSET | _displayfunction);
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();
    clear();
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LCD1602::clear()
{
    command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
    _delay_ms(2);
}

void LCD1602::home()
{
    command(LCD_RETURNHOME);  // set cursor position to zero
    _delay_ms(2);  // this command takes a long time!
}

void LCD1602::setCursor(uint8_t col, uint8_t row)
{
    int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };

    if (row >= _numlines)
        row = _numlines-1;    // we count rows starting w/0

    command(LCD_SETDDRAMADDR | col + row_offsets[row]);
}

void LCD1602::noDisplay()
{
    _displaycontrol &= ~LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD1602::display()
{
    _displaycontrol |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD1602::noCursor()
{
    _displaycontrol &= ~LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LCD1602::cursor()
{
    _displaycontrol |= LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD1602::noBlink()
{
    _displaycontrol &= ~LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD1602::blink()
{
    _displaycontrol |= LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCD1602::scrollDisplayLeft()
{
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LCD1602::scrollDisplayRight()
{
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void LCD1602::leftToRight()
{
    _displaymode |= LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LCD1602::rightToLeft()
{
    _displaymode &= ~LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LCD1602::autoscroll()
{
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LCD1602::noAutoscroll()
{
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LCD1602::send(uint8_t value, uint8_t mode)
{
    if (mode == 1)
        PORTB |= 1<<0;
    else
        PORTB &= ~(1<<0);

    write4bits(value>>4);
    write4bits(value);
}

void LCD1602::pulseEnable()
{
    PORTB &= ~(1<<1);
    _delay_us(1);
    PORTB |= 1<<1;
    _delay_us(1);    // enable pulse must be >450ns
    PORTB &= ~(1<<1);
    _delay_us(100);   // commands need > 37us to settle
}

void LCD1602::backlight(bool e)
{
    if (e)
    {
        PORTB |= 1<<2;
        _backlight = true;
    }
    else
    {
        PORTB &= ~(1<<2);
        _backlight = false;
    }
}

void LCD1602::write4bits(uint8_t value)
{
    DDRD = 0xf0;
    PORTD = value << 4;
    pulseEnable();
}


