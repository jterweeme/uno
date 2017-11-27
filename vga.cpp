// original code by Nick Gammon, Australia

// OC1B/D10: vsync 
// OC2B/D3: hsync
// TX0/D1: pixel

#include "vga.h"
#include <avr/interrupt.h>
#include "screenFont.h"
#include <stdio.h>

ISR (TIMER1_OVF_vect)
{
    VGA::g_vga->vsync();
}

ISR (TIMER2_OVF_vect)
{
    VGA::g_vga->hsync();
}

uint32_t g_vsync = 0;

void VGA::vsync()
{
    g_vsync++;
    vLine = 0;
    messageLine = 0;
    backPorchLinesToGo = verticalBackPorchLines;
}

VGA *VGA::g_vga;

void VGA::hsync()
{
    if (backPorchLinesToGo)
    {
        backPorchLinesToGo--;
        return;
    }  // end still doing back porch

    // if all lines done, do the front porch
    if (vLine >= verticalPixels)
        return;

    // pre-load pointer for speed
    const register uint8_t *linePtr = &screen_font[(vLine >> 1) & 0x07][0];
    register char *messagePtr = &(_message[messageLine][0]);
    register uint8_t i = _cols; // how many pixels to send
    UCSR0B = 1<<TXEN0;  // transmit enable (starts transmitting white)

    while (i--)
        UDR0 = pgm_read_byte(linePtr + (*messagePtr++)); // blit pixel data to screen

    // wait till done
    while (!(UCSR0A & _BV(TXC0)))
        ;

    UCSR0B = 0;   // disable transmit; drop back to black
    vLine++;    // finished this line

    // every 16 pixels it is time to move to a new line in our text
    //  (because we double up the characters vertically)
    if ((vLine & 0xF) == 0)
        messageLine++;
}

void VGA::clear()
{
    for (uint8_t row = 0; row < _rows; row++)
        for (uint8_t col = 0; col < _cols; col++)
            _message[row][col] = ' ';

    gotoxy(0, 0);
}

void VGA::init()
{
    g_vga = this;
    sei();
  
    // Timer 1 - vertical sync pulses
    DDRB |= 1<<2;
    TCCR1A = 1<<WGM10 | 1<<WGM11 | 1<<COM1B1;
    TCCR1B = 1<<WGM12 | 1<<WGM13 | 1<<CS12 | 1<<CS10;
    OCR1A = 259;  // 16666 / 64 uS = 260 (less one)
    OCR1B = 0;    // 64 / 64 uS = 1 (less one)
    TIFR1 = 1<<TOV1;   // clear overflow flag
    TIMSK1 = 1<<TOIE1;  // interrupt on overflow on timer 1

    // Timer 2 - horizontal sync pulses
    DDRD |= 1<<3;
    TCCR2A = 1<<WGM20 | 1<<WGM21 | 1<<COM2B1;
    TCCR2B = 1<<WGM22 | 1<<CS21;
    OCR2A = 63;   // 32 / 0.5 uS = 64 (less one)
    OCR2B = 7;    // 4 / 0.5 uS = 8 (less one)
    TIFR2 = 1<<TOV2;   // clear overflow flag
    TIMSK2 = 1<<TOIE2;  // interrupt on overflow on timer 2
 
    UBRR0 = 0;  // USART Baud Rate Register
    DDRD |= 1<<4;
    UCSR0B = 0;
    //UCSR0C = 1<<UMSEL00 | 1<<UMSEL01 | 1<<UCPHA0 | 1<<UCPOL0;  // Master SPI mode
    UCSR0C = 1<<UMSEL00 | 1<<UMSEL01 | 1<<UCPOL0;
}


