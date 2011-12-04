# Super Awesome LasaurGrbl python flash script.
# 
# Copyright (c) 2011 Nortd Labs
# Open Source by the terms of the Gnu Public License (GPL3) or higher.


import os


# How to use this file
# =====================

# I am on OSX with the Arduino IDE (022) installed. While I don't use the IDE
# directly I use the tool chain of the Arduino IDE.

# 1.) Define port to your Arduino One.

PORT = "/dev/tty.usbmodemfd121" 

# 2.) Make sure the following points to the avr build tools.
#     The default here is what you use if you are on OSX with
#     the Arduino IDE installed.

#OSX
AVRDUDEAPP    = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avrdude"
AVRGCCAPP     = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-gcc"
AVROBJCOPYAPP = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-objcopy"
AVRSIZEAPP    = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-size"
AVROBJDUMPAPP = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-objdump"
AVRDUDECONFIG = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/etc/avrdude.conf"

# Windows
# AVRDUDEAPP    = "C:\Program Files\arduino-0022\hardware\tools\avr\bin\avrdude"
# AVRGCCAPP     = "C:\Program Files\arduino-0022\hardware\tools\avr\bin\avr-gcc"
# AVROBJCOPYAPP = "C:\Program Files\arduino-0022\hardware\tools\avr\bin\avr-objcopy"
# AVRSIZEAPP    = "C:\Program Files\arduino-0022\hardware\tools\avr\bin\avr-size"
# AVROBJDUMPAPP = "C:\Program Files\arduino-0022\hardware\tools\avr\bin\avr-objdump"
# AVRDUDECONFIG = "C:\Program Files\arduino-0022\hardware\tools\avr\etc\avrdude.conf"


# 3.) Compile LasaurGrbl and load it to an Arduino Uno via USB
#     In the Teminal from the location of this flash.py type: 
#     python flash.py


##########################################################
# Tune the lines below only if you know what you are doing:

DEVICE = "atmega328p"
CLOCK = "16000000"
PROGRAMMER = "avrisp"    # use this for bootloader
# PROGRAMMER = "usbtiny"    # use this for programmer
BITRATE = "115200"

BUILDNAME = "LasaurGrbl"
OBJECTS  = ["main", "motion_control", "gcode", "serial", "protocol", "stepper", 
            "eeprom", "settings", "planner", "nuts_bolts", "limits", "print", "laser_control",
            "airgas_control"]
             
COMPILE = AVRGCCAPP + " -Wall -Os -DF_CPU=" + CLOCK + " -mmcu=" + DEVICE + " -I. -ffunction-sections"

for fileobj in OBJECTS:
  os.system('%(compile)s -c %(obj)s.c -o %(obj)s.o' % {'compile': COMPILE, 'obj':fileobj});
  
os.system('%(compile)s -o main.elf %(alldoto)s  -lm' % {'compile': COMPILE, 'alldoto':".o ".join(OBJECTS)+'.o'});

#os.system('rm -f %(product).hex' % {'product':BUILDNAME})

os.system('%(objcopy)s -j .text -j .data -O ihex main.elf %(product)s.hex' % {'objcopy': AVROBJCOPYAPP, 'obj':fileobj, 'product':BUILDNAME});

os.system('%(size)s *.hex *.elf *.o' % {'size':AVRSIZEAPP})

os.system('%(dude)s -c %(programmer)s -b %(bps)s -P %(port)s -p %(device)s -C %(dudeconf)s -B 10 -F -U flash:w:%(product)s.hex:i' % {'dude':AVRDUDEAPP, 'programmer':PROGRAMMER, 'bps':BITRATE, 'port':PORT, 'device':DEVICE, 'dudeconf':AVRDUDECONFIG, 'product':BUILDNAME})

# fuse setting taken over from Makefile for reference
#os.system('%(dude)s -U hfuse:w:0xd2:m -U lfuse:w:0xff:m' % {'dude':AVRDUDEAPP})
