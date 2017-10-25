#ifndef _RINGTONE_H_
#define _RINGTONE_H_
#include "misc.h"
#include <avr/pgmspace.h>

class Ringtone
{
private:
    Timer *_t2;
    const char * const _begin;
    const char * _p;
    inline char read_byte(const char *p, bool pgm) { return pgm ? pgm_read_byte(p) : *p; }
    void _play();
    uint8_t _noten(char c) const;
    void delay(uint16_t n) const;
    bool _pgm = true;
    void _readTitle();
    uint8_t _dur = 4, _oct = 6;
    uint16_t _bpm = 63;
    uint8_t _tune = 0;
public:
    char _title[10];
    Ringtone(Timer *t2, const char *tones) : _t2(t2), _begin(tones), _p(tones) { }
    void play_P() { _play(); }
    void next();
    void begin();
    void prev();
    uint8_t tune() const { return _tune; }
    void tune(uint8_t i);
    void skip() { while (read_byte(_p++, _pgm) != '\n'); }
};
#endif



