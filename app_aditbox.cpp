#include <avr/io.h>
#include "st7783.h"

#define F_CPU 16000000L
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "analog.h"
#include "button.h"

class GUI
{
private:
    MyArray<Button, 18> _buttons;
public:
    void init();
    void draw(TFT &tft);
};

void GUI::draw(TFT &tft)
{
    for (size_t i = 0; i < _buttons.size(); i++)
        _buttons.at(i).draw(tft);
}

void GUI::init()
{
    static const uint8_t x[] = {5, 90, 175};
    static const uint16_t y[] = {5, 55, 105, 155, 205, 255};

    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 3; j++)
            _buttons.at(i * 3 + j).pos(x[j], y[i]);

    for (size_t i = 0; i < _buttons.size(); i++)
    {
        _buttons.at(i).size(60, 40);
        _buttons.at(i).fill(0xf);
    }

    _buttons.at(0).label("1");
    _buttons.at(1).label("2");
    _buttons.at(2).label("3");
    _buttons.at(3).label("4");
    _buttons.at(4).label("5");
    _buttons.at(5).label("6");
    _buttons.at(6).label("7");
    _buttons.at(7).label("8");
    _buttons.at(8).label("9");
    _buttons.at(10).label("0");
    _buttons.at(15).label("C");
    _buttons.at(15).fill(0xf000);
    _buttons.at(17).label("OK");
    _buttons.at(17).fill(0x4f00);
}

int main()
{
    ST7783 tft;
    Analog analog;
    analog.init();
    tft.reset();
    tft.begin();
    tft.fillScreen(0);
    GUI gui;
    gui.init();
    gui.draw(tft);
    Serial s;
    s.init();
    char st[20];

    while (true)
    {
        DDRC &= ~(1<<3 | 1<<2);
        PORTC &= ~(1<<3 | 1<<2);
        DDRB |= 1<<0 | 1<<1;
        PORTB |= 1<<0;
        PORTB &= ~(1<<1);
        uint16_t x = analog.read(Analog::ADC1);
        sprintf(st, "%d\r\n", x);
        s.write(st);
    }


    return 0;
}


