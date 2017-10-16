// code from Nick Gammon

#include <avr/pgmspace.h>
#include "screenFont.h"
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <stdio.h>

const int horizontalBytes = 20;  // 160 pixels wide
const int verticalPixels = 480;  // 480 pixels high


const uint8_t screenFontHeight = 8;
const uint8_t screenFontWidth = 8;

const int verticalLines = verticalPixels / screenFontHeight / 2;  // double-height characters
const int horizontalPixels = horizontalBytes * screenFontWidth;

const uint8_t verticalBackPorchLines = 35;  // includes sync pulse?
const uint16_t verticalFrontPorchLines = 525 - verticalBackPorchLines;

volatile int vLine;
volatile int messageLine;
volatile uint8_t backPorchLinesToGo;

char message [verticalLines]  [horizontalBytes];
uint8_t column, line;
uint8_t x, y;  // for gotoxy


// ISR: Vsync pulse
ISR (TIMER1_OVF_vect)
{
    vLine = 0; 
    messageLine = 0;
    backPorchLinesToGo = verticalBackPorchLines;
}
  
// ISR: Hsync pulse ... this interrupt merely wakes us up
ISR (TIMER2_OVF_vect)
{
}

void doOneScanLine()
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
    const register uint8_t *linePtr = &screen_font[(vLine >> 1) & 0x07 ][0];
    register char *messagePtr = &(message[messageLine][0]);

    // how many pixels to send
    register uint8_t i = horizontalBytes;

    // turn transmitter on 
    UCSR0B = 1<<TXEN0;  // transmit enable (starts transmitting white)

    // blit pixel data to screen    
    while (i--)
        UDR0 = pgm_read_byte(linePtr + (* messagePtr++));

    // wait till done
    while (!(UCSR0A & _BV(TXC0))) 
        ;
  
    // disable transmit
    UCSR0B = 0;   // drop back to black

    // finished this line 
    vLine++;

    // every 16 pixels it is time to move to a new line in our text
    //  (because we double up the characters vertically)
    if ((vLine & 0xF) == 0)
        messageLine++;
}
   
int main()
{
    for (int i = 0; i < verticalLines; i++)
        sprintf(message[i], "Line %03i - Floris!", i);

    sei();
    TIMSK0 = 0;  // no interrupts on Timer 0
    OCR0A = 0;   // and turn it off
    OCR0B = 0;
  
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
    UCSR0C = 1<<UMSEL00 | 1<<UMSEL01 | 1<<UCPHA0 | 1<<UCPOL0;  // Master SPI mode
    set_sleep_mode(SLEEP_MODE_IDLE);  

    while (true)
    {
        sleep_mode();
        doOneScanLine();
    }

    return 0; 
}  // end of setup




