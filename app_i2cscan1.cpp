#include <avr/io.h>
#include "misc.h"
#include <stdio.h>
#include "i2c.h"

class bitset128
{
    uint8_t _buf[16] = {0};
public:
    bool test(size_t pos) const;
    void set(size_t pos);
};

bool bitset128::test(size_t pos) const
{
    return _buf[pos / 8] & (1<<(pos % 8));
}

void bitset128::set(size_t pos)
{
    _buf[pos / 8] |= 1<< pos % 8;
}

void scan(I2CBus &bus, bitset128 &b)
{
    for (uint8_t i = 1; i < 127; i++)
    {
        bus.start();
        uint8_t status = bus.write(i << 1);
        bus.stop();

        if (status == 0x18)
            b.set(i);
    }
}

void i2ctable(Serial &serial, bitset128 b)
{
    serial.write("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");
    
    for (uint8_t i = 0; i < 0x80; i += 0x10)
    {
        serial.write("?0: ");

        for (uint8_t j = 0; j < 0x10; j++)
        {
            if (b.test(i + j))
            {
                char buf[10];
                snprintf(buf, 10, "%x ", i + j);
                serial.write(buf);
            }
            else
            {
                serial.write("-- ");
            }
        }

        serial.write("\r\n");
    }
}

int main()
{
    Serial serial;
    serial.init();
    serial.write("I2C scan\r\n");
    I2CBus bus;
    bus.init(72);
    bitset128 b;
    scan(bus, b);
    i2ctable(serial, b);

    while (true)
        ;

    return 0;
}



