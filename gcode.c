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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#include <string.h>
#include <math.h>
#include "errno.h"

#include "gcode.h"
#include "config.h"
#include "input_control.h"
#include "output_control.h"
#include "stepper.h"


#define MM_PER_INCH (25.4)

#define NEXT_ACTION_DEFAULT 0
#define NEXT_ACTION_DWELL 1
#define NEXT_ACTION_GO_HOME 2
#define NEXT_ACTION_SET_COORDINATE_OFFSET 3
#define NEXT_ACTION_CANCEL 4
#define NEXT_ACTION_AIRGAS_DISABLE 5
#define NEXT_ACTION_AIR_ENABLE 6
#define NEXT_ACTION_GAS_ENABLE 7

#define MOTION_MODE_SEEK 0 // G0 
#define MOTION_MODE_LINEAR 1 // G1
#define MOTION_MODE_CANCEL 4 // G80

#define PROGRAM_FLOW_RUNNING 0
#define PROGRAM_FLOW_PAUSED 1
#define PROGRAM_FLOW_COMPLETED 2

#define STATUS_OK 0
#define STATUS_BAD_NUMBER_FORMAT 1
#define STATUS_EXPECTED_COMMAND_LETTER 2
#define STATUS_UNSUPPORTED_STATEMENT 3
#define STATUS_FLOATING_POINT_ERROR 4

#define BUFFER_LINE_SIZE 80
char rx_line[BUFFER_LINE_SIZE];

typedef struct {
  uint8_t status_code;
  uint8_t motion_mode;             // {G0, G1, G2, G3, G80}
  uint8_t inches_mode;             // 0 = millimeter mode, 1 = inches mode {G20, G21}
  uint8_t absolute_mode;           // 0 = relative motion, 1 = absolute motion {G90, G91}
  uint8_t program_flow;
  double feed_rate, seek_rate;      // Millimeters/second
  double position[3];               // Where the interpreter considers the tool to be at this point in the code
  int    nominal_laser_intensity;  // 0-255 percentage
} parser_state_t;
static parser_state_t gc;

#define FAIL(status) gc.status_code = status;

static int next_statement(char *letter, double *double_ptr, char *line, uint8_t *char_counter);
static int read_double(char *line, uint8_t *char_counter, double *double_ptr);


void gc_init() {
  memset(&gc, 0, sizeof(gc));
  gc.feed_rate = settings.default_feed_rate;
  gc.seek_rate = settings.default_seek_rate;
  gc.absolute_mode = true;
  gc.nominal_laser_intensity = LASER_OFF;   
}

static double to_millimeters(double value) {
  return(gc.inches_mode ? (value * MM_PER_INCH) : value);
}


void gc_process_line() {
    char chr = '\0';
    int numChars = 0;
    uint8_t iscomment = false;
    int status_code;
        
    while ((numChars==0) || (chr != '\n')) {
        chr = serial_read();
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
    }
    
    //// process line
    if (numChars > 0) {           // Line is complete. Then execute!
        rx_line[numChars] = '\0';    // terminate string       
        if(rx_line[0] == '$') {
            printPgmString(PSTR("\nLasaurGrbl " GRBL_VERSION));
            printPgmString(PSTR("\nSee config.h for configuration.\n"));
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
        printPgmString(PSTR("ok\n"));
        // for debugging, report back actual position
        //sprintf(tx_line,"%d, %d\r\n", st_get_position_x(), st_get_position_y());
        //serial_send_line();
    } else {
        switch(status_code) {          
            case STATUS_BAD_NUMBER_FORMAT:
            printPgmString(PSTR("Error: Bad number format\n")); break;
            case STATUS_EXPECTED_COMMAND_LETTER:
            printPgmString(PSTR("Error: Expected command letter\n")); break;
            case STATUS_UNSUPPORTED_STATEMENT:
            printPgmString(PSTR("Error: Unsupported statement\n")); break;
            case STATUS_FLOATING_POINT_ERROR:
            printPgmString(PSTR("Error: Floating point error\n")); break;
            default:
            printPgmString(PSTR("Error: "));
            printInteger(status_code);
            printPgmString(PSTR("\n"));            
        }
    }       
}


// Executes one line of 0-terminated G-Code. The line is assumed to contain only uppercase
// characters and signed floating point values (no whitespace). Comments and block delete
// characters have been removed.
uint8_t gc_execute_line(char *line) {
  uint8_t char_counter = 0;  
  char letter;
  double value;
  double unit_converted_value;
  uint8_t radius_mode = false;
  
  uint8_t absolute_override = false;          /* 1 = absolute motion for this block only {G53} */
  uint8_t next_action = NEXT_ACTION_DEFAULT;  /* The action that will be taken by the parsed line */
  
  double target[3], offset[3];  
  
  double p = 0, r = 0;
  int int_value;

  gc.status_code = STATUS_OK;
  
  // Pass 1: Commands
  while(next_statement(&letter, &value, line, &char_counter)) {
    int_value = trunc(value);
    // int_value = value;
    switch(letter) {
      case 'G':
      switch(int_value) {
        case 0: gc.motion_mode = MOTION_MODE_SEEK; break;
        case 1: gc.motion_mode = MOTION_MODE_LINEAR; break;
        case 4: next_action = NEXT_ACTION_DWELL; break;
        case 20: gc.inches_mode = true; break;
        case 21: gc.inches_mode = false; break;
        case 28: case 30: next_action = NEXT_ACTION_GO_HOME; break;
        case 53: absolute_override = true; break;
        case 80: gc.motion_mode = MOTION_MODE_CANCEL; break;        
        case 90: gc.absolute_mode = true; break;
        case 91: gc.absolute_mode = false; break;
        case 92: next_action = NEXT_ACTION_SET_COORDINATE_OFFSET; break;        
        default: FAIL(STATUS_UNSUPPORTED_STATEMENT);
      }
      break;
      
      case 'M':
      switch(int_value) {
        case 0: case 1: gc.program_flow = PROGRAM_FLOW_PAUSED; break;
        case 2: case 30: case 60: gc.program_flow = PROGRAM_FLOW_COMPLETED; break;
        case 7: next_action = NEXT_ACTION_AIR_ENABLE;break;
        case 8: next_action = NEXT_ACTION_GAS_ENABLE;break;
        case 9: next_action = NEXT_ACTION_AIRGAS_DISABLE;break;
        case 112: next_action = NEXT_ACTION_CANCEL; break;
        default: FAIL(STATUS_UNSUPPORTED_STATEMENT);
      }            
      break;
    }
    if(gc.status_code) { break; }
  }
  
  // If there were any errors parsing this line, we will return right away with the bad news
  if (gc.status_code) { return(gc.status_code); }

  char_counter = 0;
  clear_vector(target);
  clear_vector(offset);
  memcpy(target, gc.position, sizeof(target)); // i.e. target = gc.position

  // Pass 2: Parameters
  while(next_statement(&letter, &value, line, &char_counter)) {
    int_value = trunc(value);
    // int_value = value;
    unit_converted_value = to_millimeters(value);
    switch(letter) {
      case 'F': 
      if (unit_converted_value <= 0) { FAIL(STATUS_BAD_NUMBER_FORMAT); } // Must be greater than zero   
      if (gc.motion_mode == MOTION_MODE_SEEK) {
        gc.seek_rate = unit_converted_value;
      } else {
        gc.feed_rate = unit_converted_value; // millimeters per minute
      }
      break;
      case 'P': p = value; break;  // time when dwelling
      case 'S': gc.nominal_laser_intensity = value; break;      
      case 'X': case 'Y': case 'Z':
      if (gc.absolute_mode || absolute_override) {
        target[letter - 'X'] = unit_converted_value;
      } else {
        target[letter - 'X'] += unit_converted_value;
      }
      break;
    }
  }
  
  // If there were any errors parsing this line, we will return right away with the bad news
  if (gc.status_code) { return(gc.status_code); }
      
  // Perform any physical actions
  switch (next_action) {
    case NEXT_ACTION_GO_HOME: mc_go_home(); clear_vector(target); break;
    case NEXT_ACTION_DWELL: mc_dwell(p); break;   
    case NEXT_ACTION_SET_COORDINATE_OFFSET: 
    mc_set_current_position(target[X_AXIS], target[Y_AXIS], target[Z_AXIS]);
    break;
    case NEXT_ACTION_CANCEL:
    // cancel any planned blocks
    // this effectively resets the block buffer of the planer
    // but also causes the issue to void the projected any prospected positions
    mc_cancel();
    // wait for any current blocks to finish
    // after this the stepper processing goes idle
    //mc_synchronize();
    // get the actual position from the stepper processor 
    // and fix various projected positions
    mc_get_actual_position(&gc.position[X_AXIS], &gc.position[Y_AXIS], &gc.position[Z_AXIS]);    
    mc_set_current_position(gc.position[X_AXIS], gc.position[Y_AXIS], gc.position[Z_AXIS]);
    // move to the requested location
    mc_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], gc.seek_rate, LASER_OFF);
    break;
    case NEXT_ACTION_AIRGAS_DISABLE:
    mc_airgas_disable();
    break;
    case NEXT_ACTION_AIR_ENABLE:
    mc_air_enable();
    break;
    case NEXT_ACTION_GAS_ENABLE:
    mc_gas_enable();
    break;
    case NEXT_ACTION_DEFAULT: 
    switch (gc.motion_mode) {
      case MOTION_MODE_CANCEL: break;
      case MOTION_MODE_SEEK:
      mc_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], gc.seek_rate, LASER_OFF);
      break;
      case MOTION_MODE_LINEAR:
      mc_line(target[X_AXIS], target[Y_AXIS], target[Z_AXIS], gc.feed_rate, gc.nominal_laser_intensity);
      break;
    }    
  }
  
  // As far as the parser is concerned, the position is now == target. In reality the
  // motion control system might still be processing the action and the real tool position
  // in any intermediate location.
  memcpy(gc.position, target, sizeof(double)*3); // gc.position[] = target[];
  return(gc.status_code);
}


// Parses the next statement and leaves the counter on the first character following
// the statement. Returns 1 if there was a statements, 0 if end of string was reached
// or there was an error (check state.status_code).
static int next_statement(char *letter, double *double_ptr, char *line, uint8_t *char_counter) {
  if (line[*char_counter] == 0) {
    return(0); // No more statements
  }
  
  *letter = line[*char_counter];
  if((*letter < 'A') || (*letter > 'Z')) {
    FAIL(STATUS_EXPECTED_COMMAND_LETTER);
    return(0);
  }
  (*char_counter)++;
  if (!read_double(line, char_counter, double_ptr)) {
    FAIL(STATUS_BAD_NUMBER_FORMAT); 
    return(0);
  };
  return(1);
}


// Read a floating point value from a string. Line points to the input buffer, char_counter 
// is the indexer pointing to the current character of the line, while double_ptr is 
// a pointer to the result variable. Returns true when it succeeds
static int read_double(char *line, uint8_t *char_counter, double *double_ptr) {
  char *start = line + *char_counter;
  char *end;
  
  *double_ptr = strtod(start, &end);
  if(end == start) { 
    return(false); 
  };

  *char_counter = end - line;
  return(true);
}

/* 
  Intentionally not supported:

  - arcs {G2, G3}
  - Canned cycles
  - Tool radius compensation
  - A,B,C-axes
  - Multiple coordinate systems
  - Evaluation of expressions
  - Variables
  - Multiple home locations
  - Probing
  - Override control

   group 0 = {G10, G28, G30, G92, G92.1, G92.2, G92.3} (Non modal G-codes)
   group 9 = {M48, M49} enable/disable feed and speed override switches
   group 12 = {G54, G55, G56, G57, G58, G59, G59.1, G59.2, G59.3} coordinate system selection
   group 13 = {G61, G61.1, G64} path control mode
*/
