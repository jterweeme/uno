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

void Ringtone::_play(const char *p, uint8_t octave_offset, bool pgm)
{
    uint8_t default_dur = 4, default_oct = 6;
    int bpm = 63, num;
    long wholenote, duration;
    uint8_t note, scale;

    while (read_byte(p, pgm) != ':')
        p++; // ignore name

    if (read_byte(++p, pgm) == 'd')
    {
        p += 2; // skip "d="
        num = 0;

        while (isdigit(read_byte(p, pgm)))
            num = num * 10 + (read_byte(p++, pgm) - '0');

        if (num > 0)
            default_dur = num;

        p++; // skip comma
    }

    if (read_byte(p, pgm) == 'o')
    {
        p+=2; // skip "o="
        num = read_byte(p++, pgm) - '0';

        if (num >= 3 && num <= 7)
            default_oct = num;

        p++; // skip comma
    }

    if (read_byte(p, pgm) == 'b')
    {
        p+=2; // skip "b="
        num = 0;

        while (isdigit(read_byte(p, pgm)))
            num = num * 10 + (read_byte(p++, pgm) - '0');

        bpm = num;
        p++; // skip colon
    }

    wholenote = (60 * 1000L / bpm) * 4; // this is the time for whole note (in milliseconds)

    while (read_byte(p, pgm))
    {
        if (read_byte(p, pgm) == '\n')
            break;

        num = 0;

        while (isdigit(read_byte(p, pgm)))
            num = (num * 10) + (read_byte(p++, pgm) - '0');

        duration = num ? wholenote / num : wholenote / default_dur;
        note = _noten(read_byte(p++, pgm));

        if (read_byte(p, pgm) == '#')
        {
            note++;
            p++;
        }

        if (read_byte(p, pgm) == '.')
        {
            duration += duration / 2;
            p++;
        }

        scale = isdigit(read_byte(p, pgm)) ? read_byte(p++, pgm) - '0' : default_oct;
        scale += octave_offset;

        if (read_byte(p, pgm) == ',')
            p++; // skip comma for next note (or we may be at the end)

        if (note)
        {
            _t2->tone(pgm_read_word(&notes[(scale - 4) * 12 + note]));
            delay(duration);
            _t2->noTone();
        }
        else
        {
            delay(duration);
        }
    }
}


