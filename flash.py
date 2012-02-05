# Super Awesome LasaurGrbl python flash script.
# 
# Copyright (c) 2011 Nortd Labs
# Open Source by the terms of the Gnu Public License (GPL3) or higher.

import os, sys



# How to use this file
# =====================

# (1)
# Make sure you have the Arduino IDE installed (we've tested this on 022 and newer).
# While flash.py does not use the IDE directly it makes use of its tool chain.
# Please verify the following locations are correct for you platform:

if sys.platform == "darwin":  # OSX
    AVRDUDEAPP    = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avrdude"
    AVRGCCAPP     = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-gcc"
    AVROBJCOPYAPP = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-objcopy"
    AVRSIZEAPP    = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-size"
    AVROBJDUMPAPP = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-objdump"
    AVRDUDECONFIG = "/Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/etc/avrdude.conf"
    
elif sys.platform == "win32": # Windows
    AVRDUDEAPP    = "C:\\arduino\\hardware\\tools\\avr\\bin\\avrdude"
    AVRGCCAPP     = "C:\\arduino\\hardware\\tools\\avr\\bin\\avr-gcc"
    AVROBJCOPYAPP = "C:\\arduino\\hardware\\tools\\avr\\bin\\avr-objcopy"
    AVRSIZEAPP    = "C:\\arduino\\hardware\\tools\\avr\\bin\\avr-size"
    AVROBJDUMPAPP = "C:\\arduino\\hardware\\tools\\avr\\bin\\avr-objdump"
    AVRDUDECONFIG = "C:\\arduino\\hardware\\tools\\avr\\etc\\avrdude.conf"
    
elif sys.platform == "linux" or sys.platform == "linux2":  #Linux
    AVRDUDEAPP    = "avrdude"
    AVRGCCAPP     = "avr-gcc"
    AVROBJCOPYAPP = "avr-objcopy"
    AVRSIZEAPP    = "avr-size"
    AVROBJDUMPAPP = "avr-objdump"
    AVRDUDECONFIG = "/etc/avrdude.conf"
    
    
# (2)
# Define the serial port to the Lasersaur controller as the first argument to this
# script. alternatively you can create a lasaurapp.conf file with the port as 
# the first line. Also on OSX and Linux this script usually finds the right port
# automatically.


# (3)
# Compile LasaurGrbl and load it to an Arduino Uno via USB. In the Teminal/Command Line
# from the location of this flash.py type: python flash.py




# =============================================================================
# No need to edit anything below this line

SERIAL_PORT = None
CONFIG_FILE = "lasaurapp.conf"
GUESS_PPREFIX = "tty.usbmodem"


def build():
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

    os.system('%(dude)s -c %(programmer)s -b %(bps)s -P %(port)s -p %(device)s -C %(dudeconf)s -B 10 -F -U flash:w:%(product)s.hex:i' % {'dude':AVRDUDEAPP, 'programmer':PROGRAMMER, 'bps':BITRATE, 'port':SERIAL_PORT, 'device':DEVICE, 'dudeconf':AVRDUDECONFIG, 'product':BUILDNAME})

    # fuse setting taken over from Makefile for reference
    #os.system('%(dude)s -U hfuse:w:0xd2:m -U lfuse:w:0xff:m' % {'dude':AVRDUDEAPP})

    ## clean after upload
    print "Cleaning up build files."
    # current_dir = os.path.dirname(os.path.abspath(__file__))
    # current_dir = os.path.dirname(inspect.getfile(inspect.currentframe()))
    current_dir = sys.argv[0]  # we want the dir this script is running from because this is where the build files will be placed.
    for fileobj in OBJECTS:
        file_abs = os.path.join(current_dir, '%(file)s.o' % {'file':fileobj})
        if os.path.isfile(file_abs):
            os.remove(file_abs);
    file_abs = os.path.join(current_dir, 'main.elf')
    if os.path.isfile(file_abs):
        os.remove(file_abs)
    file_abs = os.path.join(current_dir, 'LasaurGrbl.hex')
    if os.path.isfile(file_abs):
        os.remove(file_abs)

## define serial port
##
if len(sys.argv) == 2:
    # (1) get the serial device from the argument list
    SERIAL_PORT = sys.argv[1]
    print "Using serial device '"+ SERIAL_PORT +"' from command line."
else:    
    if os.path.isfile(CONFIG_FILE):
        # (2) get the serial device from the config file
        fp = open(CONFIG_FILE)
        line = fp.readline().strip()
        if len(line) > 3:
            SERIAL_PORT = line
            print "Using serial device '"+ SERIAL_PORT +"' from '" + CONFIG_FILE + "'."
            
        

if not SERIAL_PORT:
    # (3) try best guess the serial device if on linux or osx
    devices = os.listdir("/dev")
    for device in devices:
        if device[:len(GUESS_PPREFIX)] == GUESS_PPREFIX:
            SERIAL_PORT = "/dev/" + device
            print "Using serial device '"+ SERIAL_PORT +"' by best guess."
            break
    
            

if SERIAL_PORT:
    build()
else:         
    print "-----------------------------------------------------------------------------"
    print "ERROR: flash.py doesn't know what serial device to connect to!"
    print "On Linux or OSX this is something like '/dev/tty.usbmodemfd121' and on"
    print "Windows this is something like 'COM1', 'COM2', 'COM3', ..."
    print "The serial port can be supplied in one of the following ways:"
    print "(1) First argument on the  command line."
    print "(2) In a config file named '" + CONFIG_FILE + "' (located in same directory)"
    print "    with the serial port string on the first line."
    print "(3) Best guess. On Linux and OSX the app can guess the serial name by"
    print "    choosing the first device it finds starting with '"+ GUESS_PPREFIX +"'."
    print "-----------------------------------------------------------------------------"
    
