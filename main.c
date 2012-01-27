/*
  main.c - An embedded CNC Controller with rs274/ngc (g-code) support
  Part of LasaurGrbl

  Copyright (c) 2009-2011 Simen Svale Skogsrud
  Copyright (c) 2011 Stefan Hechenberger


  LasaurGrbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LasaurGrbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/

#include "serial.h"
#include "config.h"
#include "planner.h"
#include "stepper.h"
#include "more_control.h"
#include "gcode.h"
#include "limits.h"


Ticker flipper;
DigitalOut led1(LED1);


void flip() {
    led1 = !led1;
}

int main(void) {
    led1 = 1;
    flipper.attach(&flip, 2.0);
  
    serial_init();
    settings_init();  
    plan_init();      
    st_init();
    laser_init();
    airgas_init();
    gc_init();
    limits_init();
                
    while(true) {
        serial_process_line();
    }    
}
