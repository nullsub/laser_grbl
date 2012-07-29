/*
  gcode.c - rs274/ngc parser.
  Part of Grbl

  Copyright (c) 2009-2011 Simen Svale Skogsrud

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef gcode_h
#define gcode_h

#include "stdint.h"

#define STATUS_OK 0
#define STATUS_BAD_NUMBER_FORMAT 1
#define STATUS_EXPECTED_COMMAND_LETTER 2
#define STATUS_UNSUPPORTED_STATEMENT 3
#define STATUS_FLOATING_POINT_ERROR 4
#define STATUS_STOP_SERIAL_REQUEST 5
#define STATUS_STOP_POWER_OFF 6
#define STATUS_STOP_CHILLER_OFF 7
#define STATUS_STOP_LIMIT_HIT 8


// Initialize the parser
void gcode_init();

// read chars from serial
void gcode_process_line();

// Execute one block of rs275/ngc/g-code
uint8_t gcode_execute_line(char *line);

// update to stepper position when steppers have been stopped
// called from the stepper code that executes the stop
void gcode_request_position_update();

#endif
