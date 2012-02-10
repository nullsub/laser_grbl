
Lasersaur - Open Source Laser cutter
-------------------------------------

This is the firmware we use for the Lasersaur. It's a slightly modified version of grbl. It's what runs on an Arduino Uno and takes g-code files to controls the stepper motors accordingly.

How to get this firmware onto an Arduino Uno? There is a python script that will do the trick. Edit the "flash.py" and follow the instruction in it. You will need a USB cable and the Arduino IDE.

For more information see the [Lasersaur Software Setup Guide](http://labs.nortd.com/lasersaur/manual/software_setup).

**DISCLAIMER:** Please be aware that operating a DIY laser cutter can be dangerous and requires full awareness of the risks involved. You build the machine and you will have to make sure it is safe. The instructions of the Lasersaur project and related software come without any warranty or guarantees whatsoever. All information is provided as-is and without claims to mechanical or electrical fitness, safety, or usefulness. You are fully responsible for doing your own evaluations and making sure your system does not burn, blind, or electrocute people.


Grbl - An embedded g-code interpreter and motion-controller for the Arduino/AVR328 microcontroller
--------------

For more information [on Grbl](https://github.com/simen/grbl)


mbed merger notes
------------------
- laser intensity, 255 or 1.0
- trunc() function in gcode parser
- NEXT_ACTION_CANCEL, newer code

TODO: motion_control, planner, stepper