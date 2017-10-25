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

class Timer
{
public:
    virtual void noToneA() const;
    virtual void toneA(uint32_t freq) const;
};

class Timer0 : public Timer
{
public:
    void noToneA() const;
    void noToneB() const;
    void toneA(uint32_t freq) const;
    void toneB(uint32_t freq) const;
    void toneCalc(uint32_t f_cpu, uint32_t freq, uint8_t &ocr, uint8_t &cs) const;
    void pwmA(uint8_t v);
    void pwmB(uint8_t v);
};

class Timer1 : public Timer
{
public:
    void noToneA() const;
    void toneA(uint32_t freq) const;
};

class Timer2 : public Timer
{
public:
    void noToneA() const;
    void toneA(uint32_t freq) const;
    void toneCalc(uint32_t f_cpu, uint32_t freq, uint8_t &ocr, uint8_t &cs) const;
};


#endif



