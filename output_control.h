/*
  motion_control.h - high level interface for issuing motion commands
  Part of Grbl

  Copyright (c) 2009-2011 Simen Svale Skogsrud
  Copyright (c) 2011 Sungeun K. Jeon
  
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

#ifndef motion_control_h
#define motion_control_h

#include <avr/io.h>

#include "config.h"
#include "planner.h"
#include "stepper.h"


// NOTE: Although the following functions structurally belongs in this module, there is nothing to do but
// to forward the request to the planner.

// Execute linear motion in absolute millimeter coordinates. Feed rate given in millimeters/second
// unless invert_feed_rate is true. Then the feed_rate means that the motion should be completed in
// (1 minute)/feed_rate time.
#define mc_line(x, y, z, feed_rate, invert_feed_rate, nominal_laser_intensity) plan_buffer_line(x, y, z, feed_rate, invert_feed_rate, nominal_laser_intensity) 

#define mc_get_actual_position(x, y, z) st_get_position(x, y, z)
#define mc_set_current_position(x, y, z) plan_set_current_position(x, y, z) 
#define mc_synchronize() st_synchronize() 

#define mc_cancel() plan_buffer_command(TYPE_CANCEL)
#define mc_airgas_disable() plan_buffer_command(TYPE_AIRGAS_DISABLE)
#define mc_air_enable() plan_buffer_command(TYPE_AIR_ENABLE)
#define mc_gas_enable() plan_buffer_command(TYPE_GAS_ENABLE)
  
// Dwell for a specific number of seconds
void mc_dwell(double seconds);

// Send the tool home (not implemented)
void mc_go_home();


void laser_init();
void set_laser_intensity(uint8_t intensity);  //0-255
void set_laser_intensity(float intensity);    //0.0-1.0 percentage


void airgas_init();
void airgas_disable();
void air_enable();
void gas_enable();


#endif
