#include "vga.h"
#include "calc.h"
#include "misc.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include "keyboard.h"

Calculator *g_calc;
Sub sub;
Div div;
Mul mul;

volatile uint32_t overflows0 = 0;

ISR(TIMER0_OVF_vect)
{
    overflows0++;
}

PS2Keyboard *g_kb;

ISR(INT0_vect)
{
    TIMSK1 &= ~(1<<TOIE1);
    TIMSK2 &= ~(1<<TOIE2);
    g_kb->isr();

    static uint8_t state = 0;
    uint8_t scancode = g_kb->get_scan_code();

    if (scancode > 0)
    {
            switch (scancode)
            {
                case PS2Keyboard::N1:
                    g_calc->push(1);
                    break;
                case PS2Keyboard::N2:
                    g_calc->push(2);
                    break;
                case PS2Keyboard::N3:
                    g_calc->push(3);
                    break;
                case PS2Keyboard::N4:
                    g_calc->push(4);
                    break;
                case PS2Keyboard::N5:
                    g_calc->push(5);
                    break;
                case PS2Keyboard::N6:
                    g_calc->push(6);
                    break;
                case PS2Keyboard::N7:
                    g_calc->push(7);
                    break;
                case PS2Keyboard::N8:
                    g_calc->push(8);
                    break;
                case PS2Keyboard::N9:
                    g_calc->push(9);
                    break;
                case PS2Keyboard::N0:
                    g_calc->push(0);
                    break;
                case PS2Keyboard::MINUS:
                    g_calc->op(&sub);
                    break;
                case PS2Keyboard::STAR:
                    g_calc->op(&mul);
                    break;
                case PS2Keyboard::SLASH:
                    g_calc->op(&div);
                    break;
                case PS2Keyboard::PLUS:
                    g_calc->add();
                    break;
                case PS2Keyboard::ENTER:
                    g_calc->equals();
                    break;
                case PS2Keyboard::ESC:
                    g_calc->reset();
                    break;
            }
    }

    TIMSK1 |= 1<<TOIE1;
    TIMSK2 |= 1<<TOIE2;
}

class OutputVGA : public Output
{
    VGA *_vga;
    char _buf[20] = {0};
    uint8_t _pos = 0;
public:
    OutputVGA(VGA *v) : _vga(v) { }
    void redraw() const;
    inline void clear() { _buf[_pos = 0] = 0; redraw(); }
    void push(char c);
};

class DummyOutput : public Output
{
public:
    void clear() { }
    void push(char c) { }
};

void OutputVGA::redraw() const
{
    _vga->clear();
    _vga->gotoxy(20 - _pos, 0);

    for (uint8_t i = 0; i <= _pos; i++)
        _vga->write(_buf[i]);
}

void OutputVGA::push(char c)
{
    if (_pos >= 19)
        return;

    _buf[_pos++] = c;
    _buf[_pos] = 0;
    redraw();
}

inline char nibble(uint8_t n)
{
    return n <= 9 ? '0' + n : 'A' + n - 10;
}

int main()
{
    sei();
    TCCR0A = 0;
    TCCR0B = 1<<CS01 | 1<<CS00;
    TCNT0 = 0;
    TIMSK0 = 1<<TOIE0;
    PS2Keyboard keyboard;
    g_kb = &keyboard;
    VGA vga;
    vga.init();
    OutputVGA cv(&vga);
    Calculator calc(&cv);
    g_calc = &calc;
    calc.reset();

    while (true)
    {
        sleep_mode();
#if 0
        static uint8_t state = 0;
        uint8_t scancode = keyboard.get_scan_code();

        if (scancode <= 0)
            continue;

        if (scancode == 0xf0)
        {
            state = PS2Keyboard::BREAK;
            continue;
        }
        
        if (state == PS2Keyboard::BREAK)
        {
            state = 0;
            continue;
        }

            switch (scancode)
            {
                case PS2Keyboard::N1:
                    calc.push(1);
                    break;
                case PS2Keyboard::N2:
                    calc.push(2);
                    break;
                case PS2Keyboard::N3:
                    calc.push(3);
                    break;
                case PS2Keyboard::N4:
                    calc.push(4);
                    break;
                case PS2Keyboard::N5:
                    calc.push(5);
                    break;
                case PS2Keyboard::N6:
                    calc.push(6);
                    break;
                case PS2Keyboard::N7:
                    calc.push(7);
                    break;
                case PS2Keyboard::N8:
                    calc.push(8);
                    break;
                case PS2Keyboard::N9:
                    calc.push(9);
                    break;
                case PS2Keyboard::N0:
                    calc.push(0);
                    break;
                case PS2Keyboard::MINUS:
                    calc.op(&sub);
                    break;
                case PS2Keyboard::STAR:
                    calc.op(&mul);
                    break;
                case PS2Keyboard::SLASH:
                    calc.op(&div);
                    break;
                case PS2Keyboard::PLUS:
                    calc.add();
                    break;
                case PS2Keyboard::ENTER:
                    calc.equals();
                    break;
                case PS2Keyboard::ESC:
                    calc.reset();
                    break;
            }
#endif
    }

    return 0;
}


