#include "i2c.h"
#include <avr/io.h>

void I2CBus::init(uint16_t brr) const
{
    TWCR = 0;
    TWAR = 0;
    TWSR = 0;
    TWBR = brr;
}

uint8_t I2CBus::start() const
{
    TWCR = 1<<TWINT | 1<<TWEN | 1<<TWSTA;
    spin(TWINT);
    return TWSR;
}

uint8_t I2CBus::write(uint8_t data) const
{
    TWDR = data;
    TWCR = 1<<TWINT | 1<<TWEN;
    spin(TWINT);
    return TWSR;
}

uint8_t I2CBus::read(uint8_t ack) const
{
    TWCR = 1<<TWINT | 1<<TWEN | (ack == 0 ? 1<<TWEA : 0);
    spin(TWINT);
    return TWDR;
}

void I2CBus::stop() const
{
    TWCR = 1<<TWINT | 1<<TWEN | 1<<TWSTO;
    spin(TWSTO);
}

void PCF8563::alarm(bool val)
{
    if (val)
        _control_status_2 |= 1<<AF;
    else
        _control_status_2 &= ~(1<<AF);
}

void PCF8563::gettime()
{
    _bus->start(0x51);
    _bus->write(0);
    _bus->stop();
    _bus->start();
    _bus->write(0x51 << 1 | 1);
    _control_status_1 = _bus->read(0);
    _control_status_2 = _bus->read(0);
    _vl_seconds = _bus->read(0);
    _min = _bus->read(0);
    _hours = _bus->read(0);
    _days = _bus->read(0);
    _weekday = _bus->read(0);
    _century_months = _bus->read(0);
    _years = _bus->read(0);
    _minute_alarm = _bus->read(0);
    _hour_alarm = _bus->read(0);
#if 0
    _day_alarm = _bus->read(0);
    _weekday_alarm = _bus->read(0);
    _clkout_control = _bus->read(0);
    _timer_control = _bus->read(0);
    _timer = _bus->read(0);
#endif
    _bus->stop();
}

void PCF8563::write()
{
    _bus->start(0x51);
    _bus->write(0);
    _bus->write(_control_status_1);
    _bus->write(_control_status_2);
    _bus->write(_vl_seconds);
    _bus->write(_min);
    _bus->write(_hours);
    _bus->write(_days);
    _bus->write(_weekday);
    _bus->write(_century_months);
    _bus->write(_years);
    _bus->write(_minute_alarm);
    _bus->stop();
}


