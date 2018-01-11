#ifndef _UNO_H_
#define _UNO_H_
#include <stdint.h>

static constexpr uint8_t
    portb_base = 0x23,
        pinb = portb_base,
        pinb0 = 0, pinb1 = 1, pinb2 = 2, pinb3 = 3, pinb4 = 4, pinb5 = 5, pinb6 = 6, pinb7 = 7,
        ddrb = portb_base + 1,
        ddb0 = 0, ddb1 = 1, ddb2 = 2, ddb3 = 3, ddb4 = 4, ddb5 = 5, ddb6 = 6, ddb7 = 7,
    portb = 0x25, pb0 = 0, pb1 = 1, pb2 = 2, pb3 = 3, pb4 = 4, pb5 = 5, pb6 = 6, pb7 = 7,
    pinc = 0x26,
    tcnt0 = 0x26,
    ocr0a = 0x27,
    ocr0b = 0x28,
    timsk0 = 0x6e,
    timsk1 = 0x6f,
    timsk2 = 0x70,
    tccr1a = 0x80, wgm10 = 0, wgm11 = 1, com1b0 = 4, com1b1 = 5, com1a0 = 6, com1a1 = 7,
    tccr1b = 0x81, cs10 = 0, cs11 = 1, cs12 = 2, wgm12 = 3, wgm13 = 4, ices1 = 6, icn1 = 7,
    tccr1c = 0x82, foc1b = 6, foc1a = 7,
    tcnt1 = 0x84,
    ocr1a = 0x88,
    ocr1al = 0x88,
    ocr1ah = 0x89,
    ocr1b = 0x8a,
    ocr1bl = 0x8a,
    ocr1bh = 0x8b,
    ucsr0a = 0xc0,
        mpcm0 = 0, u2x0 = 1, upe0 = 2, dor0 = 3, fe0 = 4, udre0 = 5, txc0 = 6, rxc0 = 7,
    ucsr0b = 0xc1,
        txb80 = 0, rxb80 = 1, ucxz02 = 2, txen0 = 3,
        rxen0 = 4, udrie0 = 5, txcie0 = 6, rxcie0 = 7,
    ucsr0c = 0xc2,
    ubrr0 = 0xc4,
    ubrr0l = 0xc4,
    ubrr0h = 0xc5,
    udr0 = 0xc6;

static volatile uint8_t
    *ppinb = (volatile uint8_t * const)pinb,
    *pddrb = (volatile uint8_t * const)ddrb,
    *pportb = (volatile uint8_t * const)portb,
    *ppinc = (volatile uint8_t * const)pinc,
    *ptcnt0 = (volatile uint8_t * const)tcnt0,
    *pocr0b = (volatile uint8_t * const)ocr0b,
    *ptimsk0 = (volatile uint8_t * const)timsk0,
    *ptimsk1 = (volatile uint8_t * const)timsk1,
    *ptccr1a = (volatile uint8_t * const)tccr1a,
    *ptccr1b = (volatile uint8_t * const)tccr1b,
    *ptccr1c = (volatile uint8_t * const)tccr1c,
    *pocr1al = (volatile uint8_t * const)ocr1al,
    *pocr1ah = (volatile uint8_t * const)ocr1ah,
    *pucsr0a = (volatile uint8_t * const)ucsr0a,
    *pucsr0b = (volatile uint8_t * const)ucsr0b,
    *pucsr0c = (volatile uint8_t * const)ucsr0c,
    *pubrr0l = (volatile uint8_t * const)ubrr0l,
    *pubrr0h = (volatile uint8_t * const)ubrr0h,
    *pudr0 = (volatile uint8_t * const)udr0;

static volatile uint16_t
    *pocr0a = (volatile uint16_t * const)ocr0a,
    *ptcnt1 = (volatile uint16_t * const)tcnt1,
    *pocr1a = (volatile uint16_t * const)ocr1a,
    *pubrr0 = (volatile uint16_t * const)ubrr0;



#endif



