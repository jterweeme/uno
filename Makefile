CC = avr-g++ --std=c++11 -mmcu=atmega328p
CMD = $(CC) -O2 -c

%.o: %.cpp
	$(CMD) $<

%.elf: %.o
	$(CC) -o $@ $^

all: app_7seg1.elf \
app_aditbox.elf \
app_analog1.elf \
app_calc1.elf \
app_calc2.elf \
app_clock1.elf \
app_dotmatrix1.elf \
app_dotmatrix2.elf \
app_eedump.elf \
app_i2cscan1.elf \
app_infrared1.elf \
app_infrared2.elf \
app_ringtone1.elf \
app_ringtone2.elf \
app_ringtone3.elf \
app_ringtoneir1.elf \
app_tone1.elf \
app_melody1.elf \
app_ps2kb1.elf \
app_pcf8563.elf \
app_rgb1.elf \
app_uartloop1.elf \
app_uartloop2.elf \
app_vga1.elf \
app_vga2.elf

app_7seg1.elf: app_7seg1.o
app_aditbox.elf: app_aditbox.o tft.o misc.o analog.o button.o
app_analog1.elf: app_analog1.o misc.o analog.o
app_calc1.elf: app_calc1.o tft.o misc.o analog.o button.o calc.o
app_calc2.elf: app_calc2.o misc.o calc.o
app_clock1.elf: app_clock1.o misc.o i2c.o lcd1602.o analog.o
app_dotmatrix1.elf: app_dotmatrix1.o
app_dotmatrix2.elf: app_dotmatrix2.o lcd1602.o
app_eedump.elf: app_eedump.o misc.o
app_i2cscan1.elf: app_i2cscan1.o misc.o i2c.o
app_infrared1.elf: app_infrared1.o misc.o infrared.o
app_infrared2.elf: app_infrared2.o misc.o infrared.o
app_rgb1.elf: app_rgb1.o misc.o
app_ringtone1.elf: app_ringtone1.o
app_ringtone2.elf: app_ringtone2.o misc.o
app_ringtone3.elf: app_ringtone3.o misc.o lcd1602.o analog.o ringtone.o
app_ringtoneir1.elf: app_ringtoneir1.o misc.o lcd1602.o ringtone.o infrared.o
app_tone1.elf: app_tone1.o
app_pcf8563.elf: app_pcf8563.o misc.o i2c.o
app_melody1.elf: app_melody1.o
app_ps2kb1.elf: app_ps2kb1.o
app_uartloop1.elf: app_uartloop1.o misc.o
app_uartloop2.elf: app_uartloop2.o misc.o
app_vga1.elf: app_vga1.o
app_vga2.elf: app_vga2.o

analog.o: analog.cpp analog.h
app_7seg1.o: app_7seg1.cpp
app_aditbox.o: app_aditbox.cpp misc.h
app_analog1.o: app_analog1.cpp misc.h
app_calc1.o: app_calc1.cpp misc.h analog.h button.h
app_calc2.o: app_calc2.cpp calc.h
app_clock1.o: app_clock1.cpp misc.h
app_dotmatrix1.o: app_dotmatrix1.cpp
app_dotmatrix2.o: app_dotmatrix2.cpp
app_eedump.o: app_eedump.cpp misc.h
app_i2cscan1.o: app_i2cscan1.cpp
app_infrared1.o: app_infrared1.cpp misc.h infrared.h
app_infrared2.o: app_infrared2.cpp misc.h infrared.h
app_ringtone1.o: app_ringtone1.cpp
app_ringtone2.o: app_ringtone2.cpp
app_ringtone3.o: app_ringtone3.cpp lcd1602.h analog.h ringtone.h misc.h
app_ringtoneir1.o: app_ringtoneir1.cpp lcd1602.h ringtone.h misc.h infrared.h
app_tone1.o: app_tone1.cpp
app_melody1.o: app_melody1.cpp
app_pcf8563.o: app_pcf8563.cpp
app_ps2kb1.o: app_ps2kb1.cpp misc.h
app_rgb1.o: app_rgb1.cpp misc.h
app_uartloop1.o: app_uartloop1.cpp misc.h
app_uartloop2.o: app_uartloop2.cpp misc.h
app_vga1.o: app_vga1.cpp
app_vga2.o: app_vga2.cpp
button.o: button.cpp
calc.o: calc.cpp
infrared.o: infrared.cpp
misc.o: misc.cpp
tft.o: tft.cpp
i2c.o: i2c.cpp
ringtone.o: ringtone.cpp ringtone.h misc.h
lcd1602.o: lcd1602.cpp

clean:
	rm -vf *.o *.gch *.elf

rebuild: clean all

upload:
	avrdude -c arduino -p m328p -P /dev/ttyACM0 -U $(UPLOAD)


