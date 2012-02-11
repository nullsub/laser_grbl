/*
  limits.h - code pertaining to limit-switches and performing the homing cycle
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
  
#include <util/delay.h>
#include <avr/io.h>
#include "config.h"
#include "stepper.h"
#include "planner.h"
#include "input_control.h"


void limit_overwrite_disable();


void limits_init() {
  LIMIT_DDR &= ~(LIMIT_MASK);
  LIMIT_PORT |= LIMIT_MASK;     //activate pull-up resistors  

  // define as output pin
  LIMIT_OVERWRITE_DDR |= 1<<LIMIT_OVERWRITE_BIT;
  limit_overwrite_disable();
}


void limit_overwrite_enable() {
  // sinking the pin overwrites the limit stop hard logic
  LIMIT_OVERWRITE_PORT &= ~(1<<LIMIT_OVERWRITE_BIT);  
}

void limit_overwrite_disable() {
  LIMIT_OVERWRITE_PORT |= (1<<LIMIT_OVERWRITE_BIT);  
}

static void homing_cycle(bool x_axis, bool y_axis, bool z_axis, bool reverse_direction, uint32_t microseconds_per_pulse) {
  
  uint32_t step_delay = microseconds_per_pulse - CONFIG_PULSE_MICROSECONDS;
  uint8_t out_bits = DIRECTION_MASK;
  uint8_t limit_bits;
  
  if (x_axis) { out_bits |= (1<<X_STEP_BIT); }
  if (y_axis) { out_bits |= (1<<Y_STEP_BIT); }
  if (z_axis) { out_bits |= (1<<Z_STEP_BIT); }
  
  // Invert direction bits if this is a reverse homing_cycle
  if (reverse_direction) {
    out_bits ^= DIRECTION_MASK;
  }
  
  // Apply the global invert mask
  out_bits ^= INVERT_MASK;
  
  // Set direction pins
  STEPPING_PORT = (STEPPING_PORT & ~DIRECTION_MASK) | (out_bits & DIRECTION_MASK);
  
  for(;;) {
    limit_bits = LIMIT_PIN;
    if (reverse_direction) {         
      // Invert limit_bits if this is a reverse homing_cycle
      limit_bits ^= LIMIT_MASK;
    }
    if (x_axis && !(limit_bits & (1<<X1_LIMIT_BIT))) {
      x_axis = false;
      out_bits ^= (1<<X_STEP_BIT);      
    }    
    if (y_axis && !(limit_bits & (1<<Y1_LIMIT_BIT))) {
      y_axis = false;
      out_bits ^= (1<<Y_STEP_BIT);
    }    
 //   if (z_axis && !(limit_bits & (1<<Z1_LIMIT_BIT))) {
 //     z_axis = false;
 //     out_bits ^= (1<<Z_STEP_BIT);
 //   }
    if(x_axis || y_axis || z_axis) {
        // step all axes still in out_bits
        STEPPING_PORT |= out_bits & STEPPING_MASK;
        _delay_us(CONFIG_PULSE_MICROSECONDS);
        STEPPING_PORT ^= out_bits & STEPPING_MASK;
        _delay_us(step_delay);
    } else { 
        return;
    }
  }
  return;
}

static void approach_limit_switch(bool x, bool y, bool z) {
  homing_cycle(x, y, z,false, 1000);
}

static void leave_limit_switch(bool x, bool y, bool z) {
  homing_cycle(x, y, z, true, 100000);
}

void limits_homing_cycle() {
  stepper_synchronize();
  // home the x and y axis
  approach_limit_switch(true, true, false);
  leave_limit_switch(true, true, false);
  planner_set_current_position(0,0,0);
}
