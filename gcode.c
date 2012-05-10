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

#include <string.h>
#include <math.h>
#include "errno.h"
#include <stdint.h>
#include <stdlib.h>
#include "gcode.h"
#include "config.h"
#include "serial.h"
#include "sense_control.h"
#include "planner.h"
#include "stepper.h"

#define MM_PER_INCH (25.4)

#define NEXT_ACTION_NONE 0
#define NEXT_ACTION_SEEK 1 
#define NEXT_ACTION_FEED 2
#define NEXT_ACTION_DWELL 3
#define NEXT_ACTION_HOMING_CYCLE 4
#define NEXT_ACTION_SET_COORDINATE_OFFSET 5
#define NEXT_ACTION_AIRGAS_DISABLE 6
#define NEXT_ACTION_AIR_ENABLE 7
#define NEXT_ACTION_GAS_ENABLE 8


#define OFFSET_G54 0
#define OFFSET_G55 1

#define BUFFER_LINE_SIZE 80
char rx_line[BUFFER_LINE_SIZE];

#define FAIL(status) gc.status_code = status;

typedef struct {
  uint8_t status_code;             // return codes
  uint8_t motion_mode;             // {G0, G1}
  bool inches_mode;                // 0 = millimeter mode, 1 = inches mode {G20, G21}
  bool absolute_mode;              // 0 = relative motion, 1 = absolute motion {G90, G91}
  double feed_rate;                // mm/min {F}
  double seek_rate;                // mm/min {F}
  double position[3];              // projected position once all scheduled motions will have been executed
  double offsets[6];               // coord system offsets {G54_X,G54_Y,G54_Z,G55_X,G55_Y,G55_Z}
  uint8_t offselect;               // currently active offset, 0 -> G54, 1 -> G55
  uint8_t nominal_laser_intensity; // 0-255 percentage
} parser_state_t;
static parser_state_t gc;

static volatile bool position_update_requested;  // make sure to update to stepper position on next occasion

// prototypes for static functions (non-accesible from other files)
static int next_statement(char *letter, double *double_ptr, char *line, uint8_t *char_counter);
static int read_double(char *line, uint8_t *char_counter, double *double_ptr);


void gcode_init() {
  memset(&gc, 0, sizeof(gc));
  gc.feed_rate = CONFIG_FEEDRATE;
  gc.seek_rate = CONFIG_SEEKRATE;
  gc.absolute_mode = true;
  gc.nominal_laser_intensity = 0U;   
  gc.offselect = 0;  // default to G54 coordinate system
  // prime G54 cs
  // refine with "G10 L2 P0 X_ Y_ Z_"
  gc.offsets[X_AXIS] = CONFIG_X_ORIGIN_OFFSET;
  gc.offsets[Y_AXIS] = CONFIG_Y_ORIGIN_OFFSET;
  gc.offsets[Z_AXIS] = CONFIG_Z_ORIGIN_OFFSET;
  // prime G55 cs
  // refine with "G10 L2 P1 X_ Y_ Z_"
  // or set to any current location with "G10 L20 P1"
  gc.offsets[3+X_AXIS] = CONFIG_X_ORIGIN_OFFSET;
  gc.offsets[3+Y_AXIS] = CONFIG_Y_ORIGIN_OFFSET;
  gc.offsets[3+Z_AXIS] = CONFIG_Z_ORIGIN_OFFSET;
  position_update_requested = false;
}


void gcode_process_line() {
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
  if (numChars > 0) {          // Line is complete. Then execute!
    rx_line[numChars] = '\0';  // terminate string
    
    // handle position update after a stop
    if (position_update_requested) {
      gc.position[X_AXIS] = stepper_get_position_x();
      gc.position[Y_AXIS] = stepper_get_position_y();
      gc.position[Z_AXIS] = stepper_get_position_z();
      position_update_requested = false;
      //printString("gcode pos update\n");  // debug
    }
    
    if (stepper_stop_requested()) {
      status_code = stepper_stop_status();
    } else if (rx_line[0] == '$') {
      printPgmString(PSTR("\nLasaurGrbl " LASAURGRBL_VERSION));
      printPgmString(PSTR("\nSee config.h for configuration.\n"));
      status_code = STATUS_OK;
    } else if (rx_line[0] == '?') {
      printString("X");
      printFloat(stepper_get_position_x());
      printString(" Y");
      printFloat(stepper_get_position_y());
      printString("\n");
      status_code = STATUS_OK;
    } else {
      // process gcode
      status_code = gcode_execute_line(rx_line);
    }    
  } else { 
    // empty or comment line, send ok for consistency
    status_code = STATUS_OK;
  }
  
  //// return status
  if (status_code == STATUS_OK) {
    printPgmString(PSTR("ok\n"));
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
      case STATUS_STOP_POWER_OFF:
        printPgmString(PSTR("Error: Power Off\n")); break;
      case STATUS_STOP_CHILLER_OFF:
        printPgmString(PSTR("Error: Chiller Off\n")); break;
      case STATUS_STOP_LIMIT_HIT:
        printPgmString(PSTR("Error: Limit Hit\n")); break;                    
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
uint8_t gcode_execute_line(char *line) {
  uint8_t char_counter = 0;  
  char letter;
  double value;
  int int_value;
  double unit_converted_value;  
  uint8_t next_action = NEXT_ACTION_NONE;
  double target[3];
  double p = 0.0;
  int cs = 0;
  int l = 0;
  gc.status_code = STATUS_OK;
    
  //// Pass 1: Commands
  while(next_statement(&letter, &value, line, &char_counter)) {
    int_value = trunc(value);
    switch(letter) {
      case 'G':
        switch(int_value) {
          case 0: gc.motion_mode = next_action = NEXT_ACTION_SEEK; break;
          case 1: gc.motion_mode = next_action = NEXT_ACTION_FEED; break;
          case 4: next_action = NEXT_ACTION_DWELL; break;
          case 10: next_action = NEXT_ACTION_SET_COORDINATE_OFFSET; break;
          case 20: gc.inches_mode = true; break;
          case 21: gc.inches_mode = false; break;
          case 30: next_action = NEXT_ACTION_HOMING_CYCLE; break;
          case 54: gc.offselect = OFFSET_G54; break;
          case 55: gc.offselect = OFFSET_G55; break;
          case 90: gc.absolute_mode = true; break;
          case 91: gc.absolute_mode = false; break;
          default: FAIL(STATUS_UNSUPPORTED_STATEMENT);
        }
        break;
      case 'M':
        switch(int_value) {
          case 7: next_action = NEXT_ACTION_AIR_ENABLE;break;
          case 8: next_action = NEXT_ACTION_GAS_ENABLE;break;
          case 9: next_action = NEXT_ACTION_AIRGAS_DISABLE;break;
          default: FAIL(STATUS_UNSUPPORTED_STATEMENT);
        }            
        break;
    }
    if (gc.status_code) { break; }
  }
  
  // bail when errors
  if (gc.status_code) { return gc.status_code; }

  char_counter = 0;
  memcpy(target, gc.position, sizeof(target)); // i.e. target = gc.position

  //// Pass 2: Parameters
  while(next_statement(&letter, &value, line, &char_counter)) {
    if (gc.inches_mode) {
      unit_converted_value = value * MM_PER_INCH;
    } else {
      unit_converted_value = value;
    }
    switch(letter) {
      case 'F':
        if (unit_converted_value <= 0) { FAIL(STATUS_BAD_NUMBER_FORMAT); }
        if (gc.motion_mode == NEXT_ACTION_SEEK) {
          gc.seek_rate = unit_converted_value;
        } else {
          gc.feed_rate = unit_converted_value;
        }
        break;
      case 'X': case 'Y': case 'Z':
        if (gc.absolute_mode) {
          target[letter - 'X'] = unit_converted_value;
        } else {
          target[letter - 'X'] += unit_converted_value;
        }
        break;        
      case 'P':  // dwelling seconds or CS selector
        if (next_action == NEXT_ACTION_SET_COORDINATE_OFFSET) {
          cs = trunc(value);
        } else {
          p = value;
        }
        break;
      case 'S':
        gc.nominal_laser_intensity = value;
        break; 
      case 'L':  // G10 qualifier 
      l = trunc(value);
        break;
    }
  }
  
  // bail when error
  if (gc.status_code) { return(gc.status_code); }
      
  //// Perform any physical actions
  switch (next_action) {
    case NEXT_ACTION_SEEK:
      planner_line( target[X_AXIS] + gc.offsets[3*gc.offselect+X_AXIS], 
                    target[Y_AXIS] + gc.offsets[3*gc.offselect+Y_AXIS], 
                    target[Z_AXIS] + gc.offsets[3*gc.offselect+Z_AXIS], 
                    gc.seek_rate, 0 );
      break;   
    case NEXT_ACTION_FEED:
      planner_line( target[X_AXIS] + gc.offsets[3*gc.offselect+X_AXIS], 
                    target[Y_AXIS] + gc.offsets[3*gc.offselect+Y_AXIS], 
                    target[Z_AXIS] + gc.offsets[3*gc.offselect+Z_AXIS], 
                    gc.feed_rate, gc.nominal_laser_intensity );                   
      break; 
    case NEXT_ACTION_DWELL:
      planner_dwell(p, gc.nominal_laser_intensity);
      break;
    // case NEXT_ACTION_STOP:
    //   planner_stop();  // stop and cancel the remaining program
    //   gc.position[X_AXIS] = stepper_get_position_x();
    //   gc.position[Y_AXIS] = stepper_get_position_y();
    //   gc.position[Z_AXIS] = stepper_get_position_z();
    //   planner_set_position(gc.position[X_AXIS], gc.position[Y_AXIS], gc.position[Z_AXIS]);
    //   // move to table origin
    //   target[X_AXIS] = 0;
    //   target[Y_AXIS] = 0;
    //   target[Z_AXIS] = 0;         
    //   planner_line( target[X_AXIS] + gc.offsets[3*gc.offselect+X_AXIS], 
    //                 target[Y_AXIS] + gc.offsets[3*gc.offselect+Y_AXIS], 
    //                 target[Z_AXIS] + gc.offsets[3*gc.offselect+Z_AXIS], 
    //                 gc.seek_rate, 0 );
    //   break;
    case NEXT_ACTION_HOMING_CYCLE:
      stepper_homing_cycle();
      // now that we are at the physical home
      // zero all the position vectors
      clear_vector(gc.position);
      clear_vector(target);
      planner_set_position(0.0, 0.0, 0.0);
      // move head to g54 offset
      gc.offselect = OFFSET_G54;
      target[X_AXIS] = 0;
      target[Y_AXIS] = 0;
      target[Z_AXIS] = 0;         
      planner_line( target[X_AXIS] + gc.offsets[3*gc.offselect+X_AXIS], 
                    target[Y_AXIS] + gc.offsets[3*gc.offselect+Y_AXIS], 
                    target[Z_AXIS] + gc.offsets[3*gc.offselect+Z_AXIS], 
                    gc.seek_rate, 0 );
      break;
    case NEXT_ACTION_SET_COORDINATE_OFFSET:
      if (cs == OFFSET_G54 || cs == OFFSET_G55) {
        if (l == 2) {
          //set offset to target, eg: G10 L2 P1 X15 Y15 Z0
          gc.offsets[3*cs+X_AXIS] = target[X_AXIS];
          gc.offsets[3*cs+Y_AXIS] = target[Y_AXIS];
          gc.offsets[3*cs+Z_AXIS] = target[Z_AXIS];
          // Set target in ref to new coord system so subsequent moves are calculated correctly.
          target[X_AXIS] = (gc.position[X_AXIS] + gc.offsets[3*gc.offselect+X_AXIS]) - gc.offsets[3*cs+X_AXIS];
          target[Y_AXIS] = (gc.position[Y_AXIS] + gc.offsets[3*gc.offselect+Y_AXIS]) - gc.offsets[3*cs+Y_AXIS];
          target[Z_AXIS] = (gc.position[Z_AXIS] + gc.offsets[3*gc.offselect+Z_AXIS]) - gc.offsets[3*cs+Z_AXIS];
          
        } else if (l == 20) {
          // set offset to current pos, eg: G10 L20 P2
          gc.offsets[3*cs+X_AXIS] = gc.position[X_AXIS] + gc.offsets[3*gc.offselect+X_AXIS];
          gc.offsets[3*cs+Y_AXIS] = gc.position[Y_AXIS] + gc.offsets[3*gc.offselect+Y_AXIS];
          gc.offsets[3*cs+Z_AXIS] = gc.position[Z_AXIS] + gc.offsets[3*gc.offselect+Z_AXIS];
          target[X_AXIS] = 0;
          target[Y_AXIS] = 0;
          target[Z_AXIS] = 0;                 
        }
      }
      break;
    case NEXT_ACTION_AIRGAS_DISABLE:
      planner_control_airgas_disable();
      break;
    case NEXT_ACTION_AIR_ENABLE:
      planner_control_air_enable();
      break;
    case NEXT_ACTION_GAS_ENABLE:
      planner_control_gas_enable();
      break;
  }
  
  // As far as the parser is concerned, the position is now == target. In reality the
  // motion control system might still be processing the action and the real tool position
  // in any intermediate location.
  memcpy(gc.position, target, sizeof(double)*3); // gc.position[] = target[];
  return gc.status_code;
}


void gcode_request_position_update() {
  position_update_requested = true;
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
  - Evaluation of expressions
  - Variables
  - Multiple home locations
  - Probing
  - Override control

*/
