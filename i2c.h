#ifndef _I2C_H_
#define _I2C_H_
#include <stdint.h>
#include <avr/io.h>

class I2CBus
{
public:
    void init(uint16_t brr = 72) const;
    uint8_t start() const;
    uint8_t restart() const;
    void stop() const;
    inline void spin(uint8_t bit) const { while (!(TWCR & 1<<bit)); }
    uint8_t write(uint8_t data) const;
    uint8_t read(uint8_t ack) const;
    inline uint8_t start(uint8_t reg) const { start(); write(reg << 1); }
};

class PCF8563
{
public:
    I2CBus *_bus;
    uint8_t _control_status_1 = 0;
    uint8_t _control_status_2 = 0;
    uint8_t _vl_seconds = 0;
    uint8_t _min = 0;
    uint8_t _hours = 0;
    uint8_t _days;
    uint8_t _weekday;
    uint8_t _century_months;
    uint8_t _years;
    uint8_t _minute_alarm;
    uint8_t _hour_alarm;
    uint8_t _day_alarm;
    uint8_t _weekday_alarm;
    uint8_t _clkout_control;
    uint8_t _timer_control;
    uint8_t _timer;
public:
    static const uint8_t AF = 3;
    static const uint8_t VL = 7;
    bool alarm() const { return _control_status_2 & 1<<AF; }
    void alarm(bool val);
    bool voltLow() const { return _vl_seconds & 1<<VL; }
    uint8_t dayOfWeek() const { return _weekday & 7; }
    uint8_t day() const { return (_days & 0xf) + (_days >> 4 & 3) * 10; }
    uint8_t decade() const { return _years >> 4; }
    uint8_t month() const { return _century_months & 0x1f; }
    uint8_t yearMod10() const { return _years & 0xf; }
    uint8_t sec() const { return (_vl_seconds >> 4 & 7) * 10 + (_vl_seconds & 0xf); }
    uint8_t min() const { return (_min >> 4 & 0x7) * 10 + (_min & 0xf); }
    uint8_t hour() const { return (_hours >> 4 & 0x3) * 10 + (_hours & 0xf); }
    uint8_t alarmMin() const { return (_minute_alarm >> 4 & 0x7) * 10 + (_minute_alarm & 0xf); }
    uint8_t alarmHour() const { return (_hour_alarm >> 4 & 0x3) * 10 + (_hour_alarm & 0xf); }
    PCF8563(I2CBus *bus) : _bus(bus) { }
    void gettime();
    void write();
};

#endif


