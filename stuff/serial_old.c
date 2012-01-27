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

#define BUFFER_SIZE 512
#define BUFFER_LINE_SIZE 80


Serial device(USBTX, USBRX);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

// ring buffers for serial TX and RX data
// used by interrupt routines
// volatile makes read-modify-write atomic 
char tx_buffer[BUFFER_SIZE];
volatile int tx_in = 0;
volatile int tx_out = 0;
char rx_buffer[BUFFER_SIZE];
volatile int rx_in = 0;
volatile int rx_out = 0;

char tx_line[BUFFER_LINE_SIZE];
char rx_line[BUFFER_LINE_SIZE];

void serial_send_line();


void rx_interrupt() {
    // read chars from serial port (UART FIFO buffer); stop if rx_buffer is full
    while ((usbserial.readable()) && (((rx_in + 1) % BUFFER_SIZE) != rx_out)) {
        rx_buffer[rx_in] = usbserial.getc();
        rx_in = (rx_in + 1) % BUFFER_SIZE;
    }
}


void tx_interrupt() {
    // write chars to serial port (UART FIFO buffer); stop if tx_buffer is empty
    while ((usbserial.writeable()) && (tx_in != tx_out)) {
        usbserial.putc(tx_buffer[tx_out]);
        tx_out = (tx_out + 1) % BUFFER_SIZE;
    }
}


void serial_init() {
    usbserial.baud(BAUD_RATE);
    // Setup a serial interrupt function to receive data
    usbserial.attach(&rx_interrupt, Serial::RxIrq);
    // Setup a serial interrupt function to transmit data
    usbserial.attach(&tx_interrupt, Serial::TxIrq);
    
    sprintf(tx_line, "\r\nLasaurGrbl %s\r\n", LASAURGRBL_VERSION);
    serial_send_line();    
}


void serial_send_line() {
    // copy tx_line to serial output buffer
    // Not sure but changed UART1_IRQn to UART1_IRQn for USB serial
    //
    int idx;
    char temp_char;
    bool empty;
    idx = 0;
        
    NVIC_DisableIRQ(UART1_IRQn);  // Start Critical Section (protect global buffer vars)
    empty = (tx_in == tx_out);
    while ((idx==0) || (tx_line[idx-1] != '\n')) {
        // if buffer full
        if (((tx_in + 1) % BUFFER_SIZE) == tx_out) {
            // wait if buffer is full; leave critical section while waiting
            NVIC_EnableIRQ(UART1_IRQn);
            while (((tx_in + 1) % BUFFER_SIZE) == tx_out) {}
            NVIC_DisableIRQ(UART1_IRQn);
        }
        tx_buffer[tx_in] = tx_line[idx];
        idx++;
        tx_in = (tx_in + 1) % BUFFER_SIZE;
    }
    if (usbserial.writeable() && (empty)) {
        temp_char = tx_buffer[tx_out];
        tx_out = (tx_out + 1) % BUFFER_SIZE;
        // Send first character to start tx interrupts, if stopped
        usbserial.putc(temp_char);
    }
    // End Critical Section
    NVIC_EnableIRQ(UART1_IRQn);
}


void serial_process_line() {
    // copy line from serial input buffer to rx_line
    // this blocks until the line is complete
    //
    int idx = 0;
    char chr;
    uint8_t iscomment = false;
        
    NVIC_DisableIRQ(UART1_IRQn);  // Start Critical Section (protect global buffer vars)
    // Loop reading rx buffer characters until end of line character
    while ((idx==0) || (rx_line[idx-1] != '\n')) {
        // if buffer empty
        if (rx_in == rx_out) {
            // Wait if buffer empty; leave critical section while waiting
            NVIC_EnableIRQ(UART1_IRQn);
            while (rx_in == rx_out) {}
            NVIC_DisableIRQ(UART1_IRQn);
        }
        chr = rx_buffer[rx_out];
        rx_out = (rx_out + 1) % BUFFER_SIZE;
        NVIC_EnableIRQ(UART1_IRQn);  // End Critical Section
        
        // process the current char; gcode-specific
        //        
        if (iscomment) {
            // Throw away all comment characters
            if (chr == ')') {
                // End of comment. Resume line.
                iscomment = false;
            }
        } else {
            if (chr <= ' ') { 
                // Throw away whitepace and control characters
            } else if (chr == '/') {
                // Disable block delete and throw away character
                // To enable block delete, uncomment following line. Will ignore until EOL.
                // iscomment = true;
            } else if (chr == '(') {
                // Enable comments flag and ignore all characters until ')' or EOL.
                iscomment = true;
            } else if (idx >= BUFFER_LINE_SIZE-1) {
                // Throw away any characters beyond the end of the line buffer
            } else if (chr >= 'a' && chr <= 'z') { // Upcase lowercase
                rx_line[idx++] = chr-'a'+'A';
            } else {
                rx_line[idx++] = chr;
            }
        }
        
    }
    
    // process line
    //
    int status_code;
    if (idx > 0) {           // Line is complete. Then execute!
        rx_line[idx-1] = 0;    // terminate string
        if(rx_line[0] == '$') {
            led3 != led3;
            // process settings command
            //status_code = settings_execute_line(rx_line);
            sprintf(tx_line, "\r\nLasaurGrbl %s\r\n", LASAURGRBL_VERSION);
            serial_send_line();
            usbserial.putc('h');
            usbserial.putc('\n');
        } else {
            // process gcode
            status_code = gc_execute_line(rx_line);
        }        
    } else { 
        // Empty or comment line
        // Send status message for syncing purposes.
        status_code = STATUS_OK;
    }
    
    // return status
    //
    if (status_code == STATUS_OK) {
        sprintf(tx_line, "ok\r\n");
        serial_send_line();
        // for debugging, report back actual position
        //sprintf(tx_line,"%d, %d\r\n", st_get_position_x(), st_get_position_y());
        //serial_send_line();
    } else {
        switch(status_code) {          
            case STATUS_BAD_NUMBER_FORMAT:
            sprintf(tx_line, "Error: Bad number format\r\n"); break;
            case STATUS_EXPECTED_COMMAND_LETTER:
            sprintf(tx_line, "Error: Expected command letter\r\n"); break;
            case STATUS_UNSUPPORTED_STATEMENT:
            sprintf(tx_line, "Error: Unsupported statement\r\n"); break;
            case STATUS_FLOATING_POINT_ERROR:
            sprintf(tx_line, "Error: Floating point error\r\n"); break;
            default:
            sprintf(tx_line, "Error: %d\r\n", status_code);
        }
        serial_send_line();
    }       
}




