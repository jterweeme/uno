#include "util.h"
#include <avr/interrupt.h>

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {};

void __cxa_pure_virtual(void) {};


size_t Prynt::write(const uint8_t *buffer, size_t size)
{
  size_t n = 0;
  while (size--) {
    n += write(*buffer++);
  }
  return n;
}

#if 0
size_t Prynt::print(const __FlashStringHelper *ifsh)
{
  const char * __attribute__((progmem)) p = (const char * ) ifsh;
  size_t n = 0;
  while (1) {
    unsigned char c = pgm_read_byte(p++);
    if (c == 0) break;
    n += write(c);
  }
  return n;
}

size_t Prynt::print(const String &s)
{
  size_t n = 0;
  for (uint16_t i = 0; i < s.length(); i++) {
    n += write(s[i]);
  }
  return n;
}
#endif

size_t Prynt::print(const char str[])
{
  return write(str);
}

size_t Prynt::print(char c)
{
  return write(c);
}

size_t Prynt::print(unsigned char b, int base)
{
  return print((unsigned long) b, base);
}

size_t Prynt::print(int n, int base)
{
  return print((long) n, base);
}

size_t Prynt::print(unsigned int n, int base)
{
  return print((unsigned long) n, base);
}

size_t Prynt::print(long n, int base)
{
  if (base == 0) {
    return write(n);
  } else if (base == 10) {
    if (n < 0) {
      int t = print('-');
      n = -n;
      return printNumber(n, 10) + t;
    }
    return printNumber(n, 10);
  } else {
    return printNumber(n, base);
  }
}


size_t Prynt::print(unsigned long n, int base)
{
  if (base == 0) return write(n);
  else return printNumber(n, base);
}

#if 0
size_t Prynt::print(double n, int digits)
{
  return printFloat(n, digits);
}

size_t Prynt::println(const __FlashStringHelper *ifsh)
{
  size_t n = print(ifsh);
  n += println();
  return n;
}
#endif

size_t Prynt::print(const Pryntable& x)
{
  return x.printTo(*this);
}

size_t Prynt::println(void)
{
  size_t n = print('\r');
  n += print('\n');
  return n;
}

#if 0
size_t Prynt::println(const String &s)
{
  size_t n = print(s);
  n += println();
  return n;
}
#endif

size_t Prynt::println(const char c[])
{
  size_t n = print(c);
  n += println();
  return n;
}

size_t Prynt::println(char c)
{
  size_t n = print(c);
  n += println();
  return n;
}

size_t Prynt::println(unsigned char b, int base)
{
  size_t n = print(b, base);
  n += println();
  return n;
}

size_t Prynt::println(int num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t Prynt::println(unsigned int num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

size_t Prynt::println(long num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}


size_t Prynt::println(unsigned long num, int base)
{
  size_t n = print(num, base);
  n += println();
  return n;
}

#if 0
size_t Prynt::println(double num, int digits)
{
  size_t n = print(num, digits);
  n += println();
  return n;
}
#endif

size_t Prynt::println(const Pryntable& x)
{
  size_t n = print(x);
  n += println();
  return n;
}
// Private Methods /////////////////////////////////////////////////////////////

size_t Prynt::printNumber(unsigned long n, uint8_t base) {
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *str = &buf[sizeof(buf) - 1];

  *str = '\0';

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  return write(str);
}

#define F_CPU 16000000UL

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

ISR(TIMER0_OVF_vect)
{
    unsigned long m = timer0_millis;
    unsigned char f = timer0_fract;

    m += MILLIS_INC;
    f += FRACT_INC;
    if (f >= FRACT_MAX) {
        f -= FRACT_MAX;
        m += 1;
    }

    timer0_fract = f;
    timer0_millis = m;
    timer0_overflow_count++;
}

unsigned long millis()
{
    unsigned long m;
    uint8_t oldSREG = SREG;
    cli();
    m = timer0_millis;
    SREG = oldSREG;
    return m;
}






