#include "st7783.h"

#define F_CPU 16000000L
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include "misc.h"

class Button
{
private:
    uint8_t _id = 0;
    uint16_t _x = 0, _y = 0, _w = 0, _h = 0, _fill = 0;
    char _label[10] = {0};
public:
    void id(uint8_t i) { _id = i; }
    uint8_t id() const { return _id; }
    void pos(uint16_t x, uint16_t y) { _x = x, _y = y; }
    void size(uint16_t w, uint16_t h) { _w = w, _h = h; }
    void fill(uint16_t color) { _fill = color; }
    inline uint16_t x() const { return _x; }
    inline uint16_t y() const { return _y; }
    inline uint16_t w() const { return _w; }
    inline uint16_t h() const { return _h; }
    void draw(TFT &tft) const;
    void label(const char *label) { strncpy(_label, label, 10); }
    bool contains(TSPoint p) const;
};

class OutputLine
{
    char _buffer[20] = {0};
    uint8_t _pos = 0;
    TFT *_tft;
public:
    OutputLine(TFT *tft) : _tft(tft) { }
    void clear() { _buffer[_pos = 0] = 0; redraw(); }
    void push(char c);
    void redraw();
};



class GUI
{
public:
    static const uint8_t ID_BTN1 = 101;
    static const uint8_t ID_BTN2 = 102;
    static const uint8_t ID_BTN3 = 103;
    static const uint8_t ID_BTN4 = 104;
    static const uint8_t ID_BTN5 = 105;
    static const uint8_t ID_BTN6 = 106;
    static const uint8_t ID_BTN7 = 107;
    static const uint8_t ID_BTN8 = 108;
    static const uint8_t ID_BTN9 = 109;
    static const uint8_t ID_BTN0 = 100;
    static const uint8_t ID_BTNC = 111;
    static const uint8_t ID_BTNADD = 112;
    static const uint8_t ID_BTNMIN = 113;
    static const uint8_t ID_BTNMUL = 114;
    static const uint8_t ID_BTNDIV = 115;
    static const uint8_t ID_BTNEQ = 110;
private:
    MyArray<Button, 18> _buttons;
    OutputLine _ol;
    TFT *_tft;
public:
    GUI(TFT *tft) : _tft(tft), _ol(tft) { }
    void init();
    void draw();
    uint8_t id(TSPoint p) const;
};

class Calculator
{
    int16_t _sum = 0;
    int16_t _register = 0;
    uint8_t _stack[50] = {0};
    uint8_t _pos = 0;
public:
    void push(uint8_t n);
};

void Calculator::push(uint8_t x)
{
    _stack[_pos++] = x;
}

void OutputLine::redraw()
{
    _tft->fillRect(0, 0, 240, 10, 0);
    _tft->drawString(240 - strlen(_buffer) * 8, 0, _buffer);
}

void Button::draw(TFT &tft) const
{
    tft.drawRect(_x, _y, _w, _h, 0xffff);
    tft.fillRect(_x + 1, _y + 1, _w - 2, _h - 2, _fill);
    tft.drawString(_x + 3, _y + 3, _label);
}

bool Button::contains(TSPoint p) const
{
    return p.x >= _x && p.x <= _x + _w && p.y >= _y && p.y <= _y + _h;
}

void GUI::draw()
{
    for (size_t i = 0; i < _buttons.size(); i++)
        _buttons.at(i).draw(*_tft);
}

uint8_t GUI::id(TSPoint p) const
{
    for (size_t i = 0; i < _buttons.size(); i++)
        if (_buttons.at(i).contains(p))
            return _buttons.at(i).id();

    return 0;
}

void OutputLine::push(char c)
{
    if (_pos >= 19)
        return;

    _buffer[_pos++] = c;
    _buffer[_pos] = 0;
    redraw();
}

struct buttons_t
{
    uint8_t x;
    uint16_t y;
    uint8_t id;
    char label[10];
    uint16_t color;
};

void GUI::init()
{
    static const buttons_t buttonmap[] = {
        {5, 25, ID_BTN1, "1", 0xf},
        {90, 25, ID_BTN2, "2", 0xf},
        {175, 25, ID_BTN3, "3", 0xf},
        {5, 75, ID_BTN4, "4", 0xf},
        {90, 75, ID_BTN5, "5", 0xf},
        {175, 75, ID_BTN6, "6", 0xf},
        {5, 125, ID_BTN7, "7", 0xf},
        {90, 125, ID_BTN8, "8", 0xf},
        {175, 125, ID_BTN9, "9", 0xf},
        {5, 175, ID_BTNADD, "*", 0xf},
        {90, 175, ID_BTN0, "0", 0xf},
        {175, 175, ID_BTNMIN, "-", 0xf},
        {5, 225, ID_BTNMUL, "*", 0xf},
        {90, 225, ID_BTNDIV, "/", 0xf},
        {175, 225, 0, 0, 0xf},
        {5, 275, ID_BTNC, "C", 0xf000},
        {90, 275, 0, 0, 0xf},
        {175, 275, ID_BTNEQ, "=", 0x4f00}
    };

    for (int i = 0; i < 18; i++)
    {
        _buttons.at(i).pos(buttonmap[i].x, buttonmap[i].y);
        _buttons.at(i).size(60, 40);
        _buttons.at(i).id(buttonmap[i].id);
        _buttons.at(i).label(buttonmap[i].label);
        _buttons.at(i).fill(buttonmap[i].color);
    }
}

int main()
{
    ST7783 tft;
    Analog analog;
    analog.init();
    tft.reset();
    tft.begin();
    tft.fillScreen(0);
    GUI gui(&tft);
    gui.init();
    gui.draw();
    Serial s;
    s.init();
    TouchScreen ts;
    OutputLine ol(&tft);
    ol.push('0');
    char st[20];
    uint8_t prev_id = 0;
    uint8_t id;
    DDRB |= 1<<5;

    while (true)
    {
        PORTB |= 1<<5;
        TSPoint p = ts.getPoint(analog);
        PORTB &= ~(1<<5);

        if (p.z > 200)
        {
            id = gui.id(p);
            _delay_ms(250);

            switch (id)
            {
                case GUI::ID_BTN1:
                    ol.push('1');
                    break;
                case GUI::ID_BTN2:
                    ol.push('2');
                    break;
                case GUI::ID_BTN3:
                    ol.push('3');
                    break;
                case GUI::ID_BTN4:
                    ol.push('4');
                    break;
                case GUI::ID_BTN5:
                    ol.push('5');
                    break;
                case GUI::ID_BTN6:
                    ol.push('6');
                    break;
                case GUI::ID_BTN7:
                    ol.push('7');
                    break;
                case GUI::ID_BTN8:
                    ol.push('8');
                    break;
                case GUI::ID_BTN9:
                    ol.push('9');
                    break;
                case GUI::ID_BTN0:
                    ol.push('0');
                    break;
                case GUI::ID_BTNC:
                    ol.clear();
                    break;
                case GUI::ID_BTNEQ:
                    ol.clear();
                    break;
            }
        }
    }


    return 0;
}


