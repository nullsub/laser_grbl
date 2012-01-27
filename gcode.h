/*
  gcode.c - rs274/ngc parser.
  Part of LasaurGrbl

  Copyright (c) 2009-2011 Simen Svale Skogsrud
  Copyright (c) 2011 Stefan Hechenberger
  Copyright (c) 2011 Sungeun K. Jeon
  
  Inspired by the Arduino GCode Interpreter by Mike Ellery and the 
  NIST RS274/NGC Interpreter by Kramer, Proctor and Messina.
  
  LasaurGrbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LasaurGrbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/


#ifndef gcode_h
#define gcode_h

#include "mbed.h"

// Initialize the parser
void gc_init();

// Execute one block of rs275/ngc/g-code
uint8_t gc_execute_line(char *line);

#endif
