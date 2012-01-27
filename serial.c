/*
  serial.c - Gcode-aware functions for sending and recieving lines via the serial port
  Part of LasaurGrbl

  Copyright (c) 2011 Stefan Hechenberger
  
  Adapted from the mbed serial interrupt example
  http://mbed.org/cookbook/Serial-Interrupts
  http://mbed.org/handbook/Serial
  http://mbed.org/handbook/SerialPC  
  
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

#include "serial.h"
#include "config.h"
#include "gcode.h"

#define BUFFER_LINE_SIZE 80
char rx_line[BUFFER_LINE_SIZE];

uint8_t xon_remote_state = 0;
#define RX_MIN_OPEN_SLOTS 32  // when to trigger XONXOFF event

MODSERIAL usbserial(USBTX, USBRX);


void serial_init() {
  usbserial.baud(BAUD_RATE);    
  usbserial.printf("\r\nLasaurGrbl %s\r\n", LASAURGRBL_VERSION);
}



void serial_process_line() {
    char chr = '\0';
    int numChars = 0;
    uint8_t iscomment = false;
    int status_code;
    // bool remote_state_is_already_XON = false;
    // bool remote_state_is_already_XOFF = false;
        
    while ((numChars==0) || (chr != '\n')) {
        chr = usbserial.getc();
        // process the current char; gcode-specific
        if (iscomment) {
            if (chr == ')') {  // ignore comment chars
                iscomment = false;  // end of comment
            }
        } else {
            if (chr <= ' ') { 
                // ignore whitepace and control characters
            } else if (chr == '(') {
                // ignore all characters until ')' or EOL.
                iscomment = true;
            } else if (numChars + 1 >= BUFFER_LINE_SIZE) {
                // reached line size, start ignoring exessive chars (+1 is for \0)
            } else if (chr >= 'a' && chr <= 'z') {
                // upcase any lower case chars
                rx_line[numChars++] = chr-'a'+'A';
            } else {
                rx_line[numChars++] = chr;
            }
        }
        
        // flow control feedback
        if (xon_remote_state == 1) {
          if (MODSERIAL_DEFAULT_RX_BUFFER_SIZE-usbserial.rxBufferGetCount() <= RX_MIN_OPEN_SLOTS) {
            // this should actually be in the rx_interrupt
            // might have to do our own serial buffering instead of using MODSERIAL
            // tx buffer is running full; let other side know by sending a XOFF
            usbserial.putc('\x13');
            // usbserial.printf("dummy XOFF");
            xon_remote_state = 0;
          }
        } else if (xon_remote_state == 0) {
          if (MODSERIAL_DEFAULT_RX_BUFFER_SIZE-usbserial.rxBufferGetCount() > RX_MIN_OPEN_SLOTS) {
            // tx buffer is running low; let other side know by sending a XON
            usbserial.putc('\x11');
            // usbserial.printf("dummy XON");
            xon_remote_state = 1;
          }
        }
    }
    
    //// process line
    if (numChars > 0) {           // Line is complete. Then execute!
        rx_line[numChars] = '\0';    // terminate string       
        if(rx_line[0] == '$') {
            // process settings command
            //status_code = settings_execute_line(rx_line);
            usbserial.printf("\nLasaurGrbl %s\n", LASAURGRBL_VERSION);
            status_code = STATUS_OK;
        } else {
            // process gcode
            status_code = gc_execute_line(rx_line);
        }        
    } else { 
        // Empty or comment line
        // Send status message for syncing purposes.
        status_code = STATUS_OK;
    }
    
    //// return status
    if (status_code == STATUS_OK) {
        usbserial.printf("ok\n");
        // for debugging, report back actual position
        //sprintf(tx_line,"%d, %d\r\n", st_get_position_x(), st_get_position_y());
        //serial_send_line();
    } else {
        switch(status_code) {          
            case STATUS_BAD_NUMBER_FORMAT:
            usbserial.printf("Error: Bad number format\n"); break;
            case STATUS_EXPECTED_COMMAND_LETTER:
            usbserial.printf("Error: Expected command letter\n"); break;
            case STATUS_UNSUPPORTED_STATEMENT:
            usbserial.printf("Error: Unsupported statement\n"); break;
            case STATUS_FLOATING_POINT_ERROR:
            usbserial.printf("Error: Floating point error\n"); break;
            default:
            usbserial.printf("Error: %d\n", status_code);
        }
    }       
}





