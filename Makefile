#  Part of Grbl
#
#  Copyright (c) 2009-2011 Simen Svale Skogsrud
#
#  Adapted for Lasersaur by Stefan Hechenberger
#
#  Grbl is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Grbl is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.


# How to use this Makefile
# ========================

# I am on OSX with the Arduino IDE (022) installed. While I don't use the IDE
# directly I point this Makefile to the tool chain of the Arduino IDE.

# 1.) Define port to your Arduino One.

PORT = /dev/tty.usbmodem621

# 2.) Make sure the following points to the avr build tools.
#     The default here is what you use if you are on OSX with
#     the Arduino IDE installed.

AVRDUDEAPP=/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avrdude
AVRGCCAPP=/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-gcc
AVROBJCOPYAPP=/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-objcopy
AVRSIZEAPP=/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-size
AVROBJDUMPAPP=/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-objdump
AVRDUDECONFIG=/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/etc/avrdude.conf

# 3.) Compile grbl and load it to an Arduino Uno via USB
#     In the Teminal from the location of the Makefile type: 
#     make flash


##########################################################
# Tune the lines below only if you know what you are doing:

DEVICE     = atmega328p
CLOCK      = 16000000
PROGRAMMER = avrisp
BAUD       = 115200

OBJECTS    = main.o motion_control.o gcode.o spindle_control.o wiring_serial.o protocol.o stepper.o \
             eeprom.o settings.o planner.o nuts_bolts.o limits.o laser_control.o
# FUSES      = -U hfuse:w:0xd9:m -U lfuse:w:0x24:m
# FUSES      = -U hfuse:w:0xd2:m -U lfuse:w:0xff:m   #atmega328
# FUSES      = -U lfuse:w:0xdf:m -U hfuse:w:0xdf:m   #amega168

AVRDUDE = $(AVRDUDEAPP) -c $(PROGRAMMER) -b $(BAUD) -P $(PORT) -p $(DEVICE) -C $(AVRDUDECONFIG) -B 10 -F
COMPILE = $(AVRGCCAPP) -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -I. -ffunction-sections

# symbolic targets:
all:	grbl.hex

.c.o:
	$(COMPILE) -c $< -o $@ 

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:grbl.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

# if you use a bootloader, change the command below appropriately:
load: all
	bootloadHID grbl.hex

clean:
	rm -f grbl.hex main.elf $(OBJECTS)

# file targets:
main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS) -lm

grbl.hex: main.elf
	rm -f grbl.hex
	$(AVROBJCOPYAPP) -j .text -j .data -O ihex main.elf grbl.hex
	$(AVROBJDUMPAPP) -h main.elf | grep .bss | ruby -e 'puts "\n\n--- Requires %s bytes of SRAM" % STDIN.read.match(/0[0-9a-f]+\s/)[0].to_i(16)'
	$(AVRSIZEAPP) *.hex *.elf *.o
# If you have an EEPROM section, you must also create a hex file for the
# EEPROM and add it to the "flash" target.

# Targets for code debugging and analysis:
disasm:	main.elf
	$(AVROBJDUMPAPP) -d main.elf

cpp:
	$(COMPILE) -E main.c 
