CC = avr-g++ --std=c++11 -mmcu=atmega328p
CMD = $(CC) -O2 -c

%.o: %.cpp
	$(CMD) $<

all: app_7seg1 \
app_calc1 \
app_clock1 \
app_dotmatrix1 \
app_dotmatrix2 \
app_eedump \
app_i2cscan1 \
app_ringtone1 \
app_ringtone2 \
app_ringtone3 \
app_tone1 \
app_melody1 \
app_ps2kb1 \
app_pcf8563 \
app_uartloop1 \
app_uartloop2 \
app_vga1 \
app_vga2

app_ringtone1: app_ringtone1.o
	$(CC) -o $@ $^

app_ringtone2: app_ringtone2.o misc.o
	$(CC) -o $@ $^

app_ringtone3: app_ringtone3.o misc.o lcd1602.o analog.o ringtone.o
	$(CC) -o $@ $^

app_tone1: app_tone1.o
	$(CC) -o $@ $^

app_pcf8563: app_pcf8563.o misc.o i2c.o
	$(CC) -o $@ $^

app_clock1: app_clock1.o misc.o i2c.o lcd1602.o analog.o
	$(CC) -o $@ $^

app_i2cscan1: app_i2cscan1.o misc.o i2c.o
	$(CC) -o app_i2cscan1 app_i2cscan1.o misc.o i2c.o

app_dotmatrix2: app_dotmatrix2.o lcd1602.o
	$(CC) -o app_dotmatrix2 app_dotmatrix2.o lcd1602.o

app_7seg1: app_7seg1.o
	$(CC) -o app_7seg1 app_7seg1.o

app_melody1: app_melody1.o
	$(CC) -o app_melody1 app_melody1.o

app_calc1: app_calc1.o tft.o misc.o analog.o
	$(CC) -o app_calc1 app_calc1.o tft.o misc.o analog.o

app_dotmatrix1: app_dotmatrix1.o
	$(CC) -o app_dotmatrix1 app_dotmatrix1.o

app_eedump: app_eedump.o misc.o
	$(CC) -o app_eedump app_eedump.o misc.o

app_ps2kb1: app_ps2kb1.o
	$(CC) -o $@ $^

app_uartloop1: app_uartloop1.o misc.o
	$(CC) -o $@ app_uartloop1.o misc.o

app_uartloop2: app_uartloop2.o misc.o
	$(CC) -o app_uartloop2 app_uartloop2.o misc.o

app_vga1: app_vga1.o
	$(CC) -o $@ $^

app_vga2: app_vga2.o
	$(CC) -o $@ $^

app_ringtone1.o: app_ringtone1.cpp
app_ringtone2.o: app_ringtone2.cpp
app_ringtone3.o: app_ringtone3.cpp
app_tone1.o: app_tone1.cpp
app_calc1.o: app_calc1.cpp
app_i2cscan1.o: app_i2cscan1.cpp
app_melody1.o: app_melody1.cpp
app_eedump.o: app_eedump.cpp
app_7seg1.o: app_7seg1.cpp
app_pcf8563.o: app_pcf8563.cpp
app_ps2kb1.o: app_ps2kb1.cpp
app_clock1.o: app_clock1.cpp
app_dotmatrix1.o: app_dotmatrix1.cpp
app_dotmatrix2.o: app_dotmatrix2.cpp
app_uartloop1.o: app_uartloop1.cpp
app_uartloop2.o: app_uartloop2.cpp
app_vga1.o: app_vga1.cpp
app_vga2.o: app_vga2.cpp
misc.o: misc.cpp
analog.o: analog.cpp
tft.o: tft.cpp
i2c.o: i2c.cpp
ringtone.o: ringtone.cpp
lcd1602.o: lcd1602.cpp

clean:
	rm -vf *.o *.gch
	rm -vf app_calc1 app_ps2kb1 app_uartloop1 app_uartloop2 app_vga1 app_vga2 app_eedump
	rm -vf app_7seg1 app_clock1 app_dotmatrix1 app_dotmatrix2 app_i2cscan1 app_pcf8563
	rm -vf app_melody1 app_ringtone1 app_ringtone2 app_ringtone3 app_tone1

rebuild: clean all

upload:
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U $(UPLOAD)


