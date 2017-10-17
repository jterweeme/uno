#ifndef _MISC_H_
#define _MISC_H_
#include <avr/io.h>
#include <string.h>

template <typename T, size_t N> struct MyArray
{
    T elems[N];
    constexpr size_t size() const { return N; }
    T &at(size_t n) const { return const_cast<T&>(elems[n]); }
};

class Analog
{
public:
    static const uint8_t ADC0 = 0;
    static const uint8_t ADC1 = 1<<MUX0;
    static const uint8_t ADC2 = 1<<MUX1;
    static const uint8_t ADC3 = 1<<MUX0 | 1<<MUX1;
    static const uint8_t ADC4 = 1<<MUX2;
    static const uint8_t ADC5 = 1<<MUX1 | 1<<MUX2;
    void init();
    uint16_t read(uint8_t input);
};

struct TSPoint
{
    uint16_t x = 0, y = 0, z = 0;
    TSPoint() { }
    TSPoint(uint16_t x1, uint16_t y1, uint16_t z1) : x(x1), y(y1), z(z1) { }
};

class TouchScreen
{
    uint16_t _threshold = 200;
public:
    static const uint8_t SAMPLES = 2;
    TSPoint getPoint(Analog &analog);
};

class Serial
{
public:
    void init() const;
    inline void enableRead() const { UCSR0B |= 1<<RXEN0; }
    inline void enableReadInterrupt() const { UCSR0B |= 1<<RXCIE0; }
    void myPutc(char c) const;
    uint8_t readByte() const;
    inline void write(const char *s) const { while (*s) myPutc(*s++); }
};


#endif



