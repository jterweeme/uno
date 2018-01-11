#ifndef _MISC_H_
#define _MISC_H_
#include <avr/io.h>
#include <string.h>
#include "uno.h"

template <typename T, size_t N> struct MyArray
{
    T elems[N];
    constexpr size_t size() const { return N; }
    T &at(size_t n) const { return const_cast<T&>(elems[n]); }
};

class CircBuf
{
    static const uint8_t BUFFER_SIZE = 5;
    volatile uint8_t _buf[BUFFER_SIZE];
public:
    volatile uint8_t head = 0, tail = 0;
    void push(uint8_t v);
    bool empty() const { return head == tail; }
    uint8_t get(uint8_t pos);
    uint8_t get();
};

class MyBitset
{
    uint8_t _bitcount = 0, _incoming = 0;
public:
    inline void reset() { _bitcount = _incoming = 0; }
    void addBit(uint8_t val);
    uint8_t count() const { return _bitcount; }
    uint8_t incoming() const { return _incoming; }
};

class Serial
{
public:
    void init() const;
    inline void enableRead() const { *pucsr0b |= 1<<rxen0; }
    inline void enableReadInterrupt() const { *pucsr0b |= 1<<rxcie0; }
    uint8_t readByte() const;
    void write(char c) const;
    inline void write(const char *s) const { while (*s) write(*s++); }
};

enum Bits { BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7 };

class Port
{
protected:
    volatile uint8_t * const pbase;
    volatile uint8_t * const pin;
public:
    volatile uint8_t * const direction;
    volatile uint8_t * const out;
    Port(uint8_t *base) : pbase(base), pin(base), direction(base + 1), out(base + 2) { }
    inline void setBit(Bits bit) { *out |= 1<<bit; }
    inline void clearBit(Bits bit) { *out &= ~(1<<bit); }
    inline void toggleBit(Bits bit) { *out ^= 1<<bit; }
    uint8_t read() { return *pin; }
    void write(uint8_t data) { *out = data; }
};

enum Direction { INPUT, OUTPUT };

struct Pin  // uses Port class
{
    Port &port;
    const Bits bit;
    Pin(Port &port, Bits bit) : port(port), bit(bit) { }
    inline void set() { port.setBit(bit); }
    inline void clear() { port.clearBit(bit); }
    inline void set(bool value) { return value ? set() : clear(); }
    inline void toggle() { port.toggleBit(bit); }
    void direction(Direction dir);
    bool read() { return port.read() & 1<<bit; }
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

class CType
{
public:
    static inline bool isUpper(char c) { return c >= 'A' && c <= 'Z'; }
    static inline bool isLower(char c) { return c >= 'a' && c <= 'z'; }
    
    static inline char convert(char c)
    {
        if (isUpper(c)) return c + 32;
        if (isLower(c)) return c - 32;
        return c;
    }
};

#endif



