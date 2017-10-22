#ifndef _RINGTONE_H_
#define _RINGTONE_H_
#include "misc.h"
#include <avr/pgmspace.h>

class Ringtone
{
private:
    Timer *_t2;
    inline char read_byte(const char *p, bool pgm) { return pgm ? pgm_read_byte(p) : *p; }
    void _play(const char *p, uint8_t octave_offset, bool pgm);
    uint8_t _noten(char c) const;
    void delay(uint16_t n) const;
public:
    Ringtone(Timer *t2) : _t2(t2) { }
    void play_P(const char *p, uint8_t octave_offset = 0) { _play(p, octave_offset, true); }
    void play(const char *p, uint8_t octave_offset = 0) { _play(p, octave_offset, false); }
};
#endif



