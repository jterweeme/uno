#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <ctype.h>
#include "misc.h"

class EEBlock
{
    uint8_t ebuf[128];
public:
    void read(const void *eeprom_addr);
    void hexDump(Serial &serial) const;
};

void EEBlock::read(const void *eeprom_addr)
{
    eeprom_read_block(ebuf, eeprom_addr, 128);
}

void EEBlock::hexDump(Serial &serial) const
{
    char wbuf[10];

    for (int i = 0; i < 128; i += 16)
    {
        for (int j = 0; j < 16; j++)
        {
            snprintf(wbuf, 10, "%02x ", ebuf[i + j]);
            serial.write(wbuf);
        }

        for (int j = 0; j < 16; j++)
        {
            if (isprint(ebuf[i + j]))
                serial.myPutc(ebuf[i + j]);
            else
                serial.myPutc('.');
        }

        serial.write("\r\n");
    }   
}

int main()
{
    Serial serial;
    serial.init();
    EEBlock eeblock;

    for (uint16_t i = 0; i < 1024; i += 128)
    {
        eeblock.read((const void *)i);
        eeblock.hexDump(serial);
    }

    while (true)
    {
    }

    return 0;
}


