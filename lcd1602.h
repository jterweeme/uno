#ifndef _LCD1602_H_
#define _LCD1602_H
#include <stdint.h>
#include <stddef.h>

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
static constexpr uint8_t LCD_DISPLAYMOVE = 8;
static constexpr uint8_t LCD_CURSORMOVE = 0;
static constexpr uint8_t LCD_MOVERIGHT = 4;
static constexpr uint8_t LCD_MOVELEFT = 0;

// flags for function set
static constexpr uint8_t LCD_8BITMODE = 0x10;
static constexpr uint8_t LCD_4BITMODE = 0x00;
static constexpr uint8_t LCD_2LINE = 0x08;
static constexpr uint8_t LCD_1LINE = 0x00;
static constexpr uint8_t LCD_5x10DOTS = 0x04;
static constexpr uint8_t LCD_5x8DOTS = 0x00;

class LCD1602
{
private:
    void send(uint8_t, uint8_t);
    void write4bits(uint8_t);
    void pulseEnable();
    uint8_t _displayfunction;
    uint8_t _displaycontrol;
    uint8_t _displaymode;
    uint8_t _initialized;
    uint8_t _numlines,_currline;
    bool _backlight = true;
public:
    void init();
    void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
    void clear();
    void home();
    void noDisplay();
    void display();
    void noBlink();
    void blink();
    void noCursor();
    void cursor();
    void scrollDisplayLeft();
    void scrollDisplayRight();
    void leftToRight();
    void rightToLeft();
    void autoscroll();
    void noAutoscroll();
    void backlight(bool e);
    void backlight() { backlight(_backlight ? false : true); }
    void setCursor(uint8_t, uint8_t);
    inline void write(uint8_t val) { send(val, 1); }
    inline void write(const char *s) { while (*s) write(*s++); }
    inline void command(uint8_t cmd) { send(cmd, 0); }
};
#endif

