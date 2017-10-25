#include "ringtone.h"

#define F_CPU 16000000UL
#include <util/delay.h>
#include <ctype.h>

void Ringtone::delay(uint16_t n) const
{
    for (int i = 0; i < n; i++)
        _delay_ms(1);
}

uint8_t Ringtone::_noten(char c) const
{
    switch (c)
    {
    case 'c':
        return 1;
    case 'd':
        return 3;
    case 'e':
        return 5;
    case 'f':
        return 6;
    case 'g':
        return 8;
    case 'a':
        return 10;
    case 'b':
        return 12;
    case 'p':
    default:
        return 0;
    }
}

const uint16_t notes[] PROGMEM =
{
    0, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554,
    587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245,
    1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489,
    2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 2*2093, 2*2217, 2*2349,
    2*2489, 2*2637, 2*2794, 2*2960, 2*3136, 2*3322, 2*3520, 2*3729, 2*3951, 0
};

void Ringtone::prev()
{
    tune(tune() - 1);
}

void Ringtone::begin()
{
    _tune = 0;
    _p = _begin;
}

void Ringtone::tune(uint8_t t)
{
    begin();
    
    for (uint8_t i = 1; i < t; i++)
    {
        skip();
        _tune++;
    }
}

void Ringtone::next()
{
    _tune++;
    _readTitle();

    if (read_byte(++_p, _pgm) == 'd')
    {
        _p += 2; // skip "d="
        int num = 0;

        while (isdigit(read_byte(_p, _pgm)))
            num = num * 10 + (read_byte(_p++, _pgm) - '0');

        if (num > 0)
            _dur = num;

        _p++; // skip comma
    }

    if (read_byte(_p, _pgm) == 'o')
    {
        _p+=2; // skip "o="
        uint8_t num = read_byte(_p++, _pgm) - '0';

        if (num >= 3 && num <= 7)
            _oct = num;

        _p++; // skip comma
    }

    if (read_byte(_p, _pgm) == 'b')
    {
        _p+=2; // skip "b="
        int num = 0;

        while (isdigit(read_byte(_p, _pgm)))
            num = num * 10 + (read_byte(_p++, _pgm) - '0');

        _bpm = num;
        _p++; // skip colon
    }

}

void Ringtone::_readTitle()
{
    uint8_t i = 0;

    for (char c; (c = read_byte(_p, _pgm)) != ':'; _p++)
        if (i < 10)
            _title[i++] = c;

    for (; i < 10; i++)
        _title[i] = ' ';    // clear remnants of previous use
}

void Ringtone::_play()
{
    long wholenote = (60 * 1000L / _bpm) * 4; // this is the time for whole note (in milliseconds)

    while (read_byte(_p, _pgm))
    {
        if (read_byte(_p, _pgm) == '\n')
            break;

        int num = 0;

        while (isdigit(read_byte(_p, _pgm)))
            num = (num * 10) + (read_byte(_p++, _pgm) - '0');

        long duration = num ? wholenote / num : wholenote / _dur;
        uint8_t note = _noten(read_byte(_p++, _pgm));

        if (read_byte(_p, _pgm) == '#')
        {
            note++;
            _p++;
        }

        if (read_byte(_p, _pgm) == '.')
        {
            duration += duration / 2;
            _p++;
        }

        uint8_t scale = isdigit(read_byte(_p, _pgm)) ? read_byte(_p++, _pgm) - '0' : _oct;

        if (read_byte(_p, _pgm) == ',')
            _p++; // skip comma for next note (or we may be at the end)

        if (note)
        {
            _t2->toneA(pgm_read_word(&notes[(scale - 4) * 12 + note]));
            delay(duration);
            _t2->noToneA();
        }
        else
        {
            delay(duration);
        }
    }
}


