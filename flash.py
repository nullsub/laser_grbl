# Super Awesome LasaurGrbl python flash script.
# 
# Copyright (c) 2011 Nortd Labs
# Open Source by the terms of the Gnu Public License (GPL3) or higher.


import os


# How to use this file
# =====================

# To compile and upload the Lasersaur firmware a couple of tools are 
# required (avr-gcc, avr-libc, and avrdude). On OSX and Windows the simplest 
# way to get them is to install the Arduino IDE. On Linux these package can
# easily be installed with the package manager (also installing "arduino-core"
# should work nicely).

# 1.) Define port to your Arduino One.
#     You can find this out in the Arduino IDE under Tools->Serial Port
#     On OSX you can also do a "ls /dev/tty.usbmodem*" in the Terminal

PORT = "/dev/tty.usbmodem621"
#PORT = "COM1"

# 2.) Make sure the following points to the avr build tools.
#     On Windows your installation folder may vary.

#OSX
AVRDUDEAPP    = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avrdude"
AVRGCCAPP     = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-gcc"
AVROBJCOPYAPP = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-objcopy"
AVRSIZEAPP    = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-size"
AVROBJDUMPAPP = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-objdump"
AVRDUDECONFIG = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/etc/avrdude.conf"

# Windows (installation path may vary)
# AVRDUDEAPP    = "C:\\arduino\\hardware\\tools\\avr\\bin\\avrdude"
# AVRGCCAPP     = "C:\\arduino\\hardware\\tools\\avr\\bin\\avr-gcc"
# AVROBJCOPYAPP = "C:\\arduino\\hardware\\tools\\avr\\bin\\avr-objcopy"
# AVRSIZEAPP    = "C:\\arduino\\hardware\\tools\\avr\\bin\\avr-size"
# AVROBJDUMPAPP = "C:\\arduino\\hardware\\tools\\avr\\bin\\avr-objdump"
# AVRDUDECONFIG = "C:\\arduino\\hardware\\tools\\avr\\etc\\avrdude.conf"

# Linux (Ubuntu)
# AVRDUDEAPP    = "avrdude"
# AVRGCCAPP     = "avr-gcc"
# AVROBJCOPYAPP = "avr-objcopy"
# AVRSIZEAPP    = "avr-size"
# AVROBJDUMPAPP = "avr-objdump"
# AVRDUDECONFIG = "/etc/avrdude.conf"

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
            "eeprom", "settings", "planner", "nuts_bolts", "limits", "print", "laser_control"]
             
COMPILE = AVRGCCAPP + " -Wall -Os -DF_CPU=" + CLOCK + " -mmcu=" + DEVICE + " -I. -ffunction-sections"

for fileobj in OBJECTS:
  os.system('%(compile)s -c %(obj)s.c -o %(obj)s.o' % {'compile': COMPILE, 'obj':fileobj});
  
os.system('%(compile)s -o main.elf %(alldoto)s  -lm' % {'compile': COMPILE, 'alldoto':".o ".join(OBJECTS)+'.o'});

#os.system('rm -f %(product).hex' % {'product':BUILDNAME})

os.system('%(objcopy)s -j .text -j .data -O ihex main.elf %(product)s.hex' % {'objcopy': AVROBJCOPYAPP, 'product':BUILDNAME});

os.system('%(size)s *.hex *.elf *.o' % {'size':AVRSIZEAPP})

os.system('%(dude)s -c %(programmer)s -b %(bps)s -P %(port)s -p %(device)s -C %(dudeconf)s -B 10 -F -U flash:w:%(product)s.hex:i' % {'dude':AVRDUDEAPP, 'programmer':PROGRAMMER, 'bps':BITRATE, 'port':PORT, 'device':DEVICE, 'dudeconf':AVRDUDECONFIG, 'product':BUILDNAME})

# fuse setting taken over from Makefile for reference
#os.system('%(dude)s -U hfuse:w:0xd2:m -U lfuse:w:0xff:m' % {'dude':AVRDUDEAPP})
