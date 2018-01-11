#include <avr/io.h>

static void spiSend(uint8_t b)
{
    SPDR = b;
    while (!(SPSR & 1<<SPIF));
}

static uint8_t spiRec(void)
{
    spiSend(0XFF);
    return SPDR;
}

int main()
{
    DDRB |= 1<<1;
    PORTB &= ~(1<<1);

    for (uint8_t j = 0; j <= 0xff; j++)
        for (uint8_t i = 0; i <= 0xff; i++)
            spiSend(i);

    while (true)
        ;

    return 0;
}


