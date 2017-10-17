/*
D4 = D4 = PORTD4
D5 = D5 = PORTD5
D6 = D6 = PORTD6
D7 = D7 = PORTD7
RS = D8 = PORTB0
E = D9 = PORTB1
*/

#include <avr/interrupt.h>

#define F_CPU 16000000L
#include <util/delay.h>
#include <stdio.h>

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
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class LiquidCrystal {
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
    void setCursor(uint8_t, uint8_t);
    size_t write(uint8_t);
    void write(const char *s) { while (*s) write(*s++); }
    void command(uint8_t);
private:
  void send(uint8_t, uint8_t);
  void write4bits(uint8_t);
  void pulseEnable();

  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;

  uint8_t _initialized;

  uint8_t _numlines,_currline;
};

void LiquidCrystal::init()
{
    DDRB |= 1<<0 | 1<<1;
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    begin(16, 1);
}

void LiquidCrystal::begin(uint8_t cols, uint8_t lines, uint8_t dotsize)
{
    if (lines > 1)
        _displayfunction |= LCD_2LINE;

    _numlines = lines;
    _currline = 0;

    if ((dotsize != 0) && (lines == 1))
        _displayfunction |= LCD_5x10DOTS;

    _delay_us(50000);
    PORTB &= ~(1<<0);   // RS
    PORTB &= ~(1<<1);
    write4bits(0x03);
    _delay_us(4500); // wait min 4.1ms
    write4bits(0x03);   // second try
    _delay_us(4500); // wait min 4.1ms
    write4bits(0x03);   // third go!
    _delay_us(150);
    write4bits(0x02);
    command(LCD_FUNCTIONSET | _displayfunction);
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();
    clear();
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal::clear()
{
    command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
    _delay_us(2000);
}

void LiquidCrystal::home()
{
    command(LCD_RETURNHOME);  // set cursor position to zero
    _delay_us(2000);  // this command takes a long time!
}

void LiquidCrystal::setCursor(uint8_t col, uint8_t row)
{
    int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };

    if (row >= _numlines)
        row = _numlines-1;    // we count rows starting w/0

    command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}
void LiquidCrystal::noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal::noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal::cursor() {
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal::noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal::scrollDisplayLeft(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal::scrollDisplayRight(void) {
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal::leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal::rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal::autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal::noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

inline void LiquidCrystal::command(uint8_t value) {
  send(value, 0);
}

inline size_t LiquidCrystal::write(uint8_t value) {
  send(value, 1);
  return 1; // assume sucess
}

void LiquidCrystal::send(uint8_t value, uint8_t mode)
{
    if (mode == 1)
        PORTB |= 1<<0;
    else
        PORTB &= ~(1<<0);

    write4bits(value>>4);
    write4bits(value);
}

void LiquidCrystal::pulseEnable()
{
    PORTB &= ~(1<<1);
    _delay_us(1);
    PORTB |= 1<<1;
    _delay_us(1);    // enable pulse must be >450ns
    PORTB &= ~(1<<1);
    _delay_us(100);   // commands need > 37us to settle
}

void LiquidCrystal::write4bits(uint8_t value)
{
    DDRD = 0xf0;
    PORTD = value << 4;
    pulseEnable();
}



int main()
{
    LiquidCrystal lcd;
    lcd.init();
    lcd.begin(16,2);
    lcd.write("Apollo");

    while (true)
    {
    }

    return 0;
}



