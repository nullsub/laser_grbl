/*
  serial.c - Gcode-aware functions for sending and recieving lines via the serial port
  Part of LasaurGrbl

  Copyright (c) 2011 Stefan Hechenberger
  
  Adapted from the mbed serial interrupt example
  http://mbed.org/cookbook/Serial-Interrupts
  
  Also adapted from Grbl's protocol module.
  
  LasaurGrbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LasaurGrbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/

#ifndef serial_h
#define serial_h

#include "mbed.h"

#define STATUS_OK 0
#define STATUS_BAD_NUMBER_FORMAT 1
#define STATUS_EXPECTED_COMMAND_LETTER 2
#define STATUS_UNSUPPORTED_STATEMENT 3
#define STATUS_FLOATING_POINT_ERROR 4

#define MODSERIAL_DEFAULT_RX_BUFFER_SIZE 256
#define MODSERIAL_DEFAULT_TX_BUFFER_SIZE 256 
#include "MODSERIAL.h"

extern MODSERIAL usbserial;

void serial_init();
void serial_process_line();
void serial_printf( char *msg, ... );


#endif
