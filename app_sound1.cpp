/*
credits to Tomasz Wisniewski
pcarduino.blogspot.com
github.com/dagon666
*/

#include <stdint.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/crc16.h>

#ifndef F_CPU
#define F_CPU 16000000
#endif

#define SERIAL_COLLECT_STATS 0
#define SERIAL_IMPLEMENT_RX_INT 1
#define SERIAL_IMPLEMENT_TX_INT 0
#define SERIAL_SUPPORT_HIGH_SPEEDS 1
#define SERIAL_STDIN_POLL 0
#define SERIAL_STDOUT_POLL 1
#define SERIAL_RX_RING_SIZE 64
#define SERIAL_TX_RING_SIZE 64
#define TDELAY_IMPLEMENT_T0_INT 0
#define TDELAY_IMPLEMENT_T1_INT 0
#define TDELAY_IMPLEMENT_T2_INT 0

typedef struct _ring_buffer
{
    volatile uint8_t head;
    volatile uint8_t tail;
    volatile uint8_t ring[1];
} ring_buffer;

#define RING_SIZE (sizeof(ring_buffer) - 1)

typedef enum _e_serial_flags {
    E_FLAGS_SERIAL_RX_INTERRUPT = 0x01,
    E_FLAGS_SERIAL_TX_INTERRUPT,
} e_serial_flags;

typedef struct _t_buffer {

    union {
        volatile uint8_t raw[SERIAL_RX_RING_SIZE + RING_SIZE];
        volatile ring_buffer r;
    } u;
} t_buffer;

unsigned int serial_recv(void *a_data, unsigned int a_size, unsigned char a_waitall);
uint8_t serial_getc(unsigned char *a_data);
volatile t_buffer* serial_get_rx_state();

#define MODE_8K_8B 0
#define MODE_8K_16B 1
#define MODE_16K_8B 2
#define MODE_18K_8B 3
#define MODE MODE_16K_8B
#define PWM_SAMPLES_BUFFER 128
#define PWM_BUFFERS 4

static volatile t_buffer g_rx_buff;
static volatile t_buffer g_tx_buff;
static volatile uint16_t g_head = 0x00, g_tail = 0x00;
static volatile uint16_t max_samples = 0x00;

struct packet {
	uint16_t crc16;
	uint8_t num;
	uint8_t samples[PWM_SAMPLES_BUFFER];
};

static volatile struct packet p[PWM_BUFFERS];

static uint8_t zerial_getc(uint8_t *a_data)
{
    if (g_rx_buff.u.r.head == g_rx_buff.u.r.tail)
        return 0;

    *a_data = g_rx_buff.u.r.ring[g_rx_buff.u.r.tail];
    g_rx_buff.u.r.tail = (g_rx_buff.u.r.tail + 1) % SERIAL_RX_RING_SIZE;
    return 1;
}

#define SLIP_END  0300
#define SLIP_ESC  0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335

uint8_t slip_verify_crc16(uint8_t *a_buff, uint8_t a_buflen, uint8_t a_crcpos) {

    uint16_t crc_recv = 0x00;
    uint16_t crc_calcd = 0x00;

    if ((a_crcpos + 2) > a_buflen || !a_buflen)
        return 0;

    crc_recv = (*(uint16_t *)&a_buff[a_crcpos]);
    (*(uint16_t *)&a_buff[a_crcpos]) = 0x0000;

    while (a_buflen)
    {
        crc_calcd = _crc16_update(crc_calcd, *a_buff);
        a_buff++;
        a_buflen--;
    }

    return crc_calcd == crc_recv ? crc_calcd : 0;
}


static uint8_t zlip_recv(uint8_t *a_buff, uint8_t a_buflen)
{
    uint8_t c = 0;
    uint16_t recv = 0;
    uint8_t escape = 0xff;

    while (1)
    {
        if (zerial_getc(&c) == 0)
            continue;

        switch (c)
        {
        case 0300:
            if (recv)
                return recv;

            break;
        case 0333:
            escape = 0;
            break;
        default:
            if (!escape)
            {
                c = (c == SLIP_ESC_END ? SLIP_END : (c == SLIP_ESC_ESC ? SLIP_ESC : c));
                escape = 0xff;
            }

            if (recv < a_buflen)
                a_buff[recv++] = c;

            break;
        }
    }
}

static void serial_send(uint8_t c)
{
    while ((UCSR0A & 1<<UDRE0) == 0)
        ;

    UDR0 = c;
}

static void serial_flush()
{
    unsigned char dummy __attribute__((unused)) = 0x00;

    while (UCSR0A & 1<<RXC0)
        dummy = UDR0;
}

ISR(USART_UDRE_vect, ISR_BLOCK)
{
    if (g_tx_buff.u.r.head != g_tx_buff.u.r.tail)
    {
        UDR0 = g_tx_buff.u.r.ring[g_tx_buff.u.r.tail];
        g_tx_buff.u.r.tail = (g_tx_buff.u.r.tail + 1) % SERIAL_TX_RING_SIZE;

    }
    else
    {
        UCSR0B &= ~_BV(UDRIE0);
    }
}

ISR(USART_RX_vect, ISR_BLOCK)
{
    if (bit_is_clear(UCSR0A, FE0))
    {
        volatile uint8_t data = UDR0;
        volatile uint8_t next = (g_rx_buff.u.r.head + 1) % SERIAL_RX_RING_SIZE;

        if (next != g_rx_buff.u.r.tail)
        {
            g_rx_buff.u.r.ring[g_rx_buff.u.r.head] = data;
            g_rx_buff.u.r.head = next;          
        }
    }
    else
    {
        volatile uint8_t data __attribute__((unused)) = UDR0;
    }
}

static inline void serial_collect_samples(volatile struct packet *a_data)
{
    uint16_t avail = (max_samples + g_head - g_tail) % max_samples;

    if (avail >= 384)
        serial_send('W');

    uint8_t size = zlip_recv((uint8_t *)a_data, sizeof(struct packet));

    if (size < 4)
        return;

    if (!slip_verify_crc16((uint8_t *)a_data, size, 0))
        return;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        g_head = (g_head + a_data->num) % max_samples;
    }
}

ISR(TIMER0_COMPA_vect, ISR_NOBLOCK)
{
	if (g_tail != g_head)
    {
		OCR1AH = 0x00;
		OCR1BH = 0x00;
		OCR1AL = p[g_tail >> 7].samples[g_tail & 0x7f];
		g_tail = (g_tail + 1) % max_samples;
	}
}

static const uint8_t _g_prescalers01[] = { 0x00, 0x03, 0x06, 0x08, 0x0a, 0x00 };

static uint32_t _timer_freq_prezcale(uint32_t a_freq, uint16_t a_criterion)
{
    uint32_t retval = 0x00;
    uint16_t *ocr = (uint16_t *)&retval;
    uint8_t *presc = ((uint8_t *)&retval) + 3;
    uint32_t ocr2 = 0;
    const uint8_t *prescalers = _g_prescalers01;

    do
    {
        ocr2 = (uint32_t)(F_CPU / ((a_freq << 1) * (0x01 << prescalers[*presc])));

        if (ocr2)
            --ocr2;

        ++*presc;
    }
    while ((ocr2 > a_criterion) && (prescalers[*presc]));

    if (ocr2 > a_criterion)
        ocr2 = a_criterion;

    *ocr = ocr2 & 0xffff;
    return retval;
}

int main(void)
{
    UCSR0A = 0x00;
    UCSR0A |= 1<<U2X0;
    UBRR0L = 8;
    UCSR0C |= 0x06;
    UCSR0B |= 1<<RXEN0 | 1<<TXEN0;
    serial_flush();
    UCSR0B &= ~(1<<TXCIE0);
    UCSR0B |= 1<<RXCIE0;
    sei();
	serial_flush();
	max_samples = PWM_SAMPLES_BUFFER * PWM_BUFFERS;
    TCCR1A = 0;
    TCCR1B = 0x10;
    OCR1A = 0;
    TCCR1A |= 0x80;
    OCR1AH = OCR1AL = 0;
    DDRB |= 1<<DDB1;
    TCCR1B |= 0x01;
    ICR1 = (1<<8) - 1;
    TCCR0A = 0x02;
    TCCR0B = 0;
    TCNT0 = 0;
    OCR0A = 0;
	uint32_t pocr = _timer_freq_prezcale(8000, 255);
    TCCR0B &= 0xf8;
    TCCR0B |= pocr >> 24 & 0x07;
    OCR0A = pocr & 0xff;
    TCNT0 = 0;
    TIMSK0 |= 1<<OCIE0A;

	while (1)
    {
        serial_collect_samples(&p[g_head>>7]);
    }
	return 0;
}



