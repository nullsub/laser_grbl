/*
  io_control.h - controlling sensors and actuators
  Part of LasaurGrbl

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

#ifndef io_control_h
#define io_control_h

#include <stdbool.h>


void sense_init();
#define SENSE_POWER ((SENSE_PORT >> POWER_BIT) & 1)
#define SENSE_CHILLER ((SENSE_PORT >> CHILLER_BIT) & 1)
#define SENSE_DOOR ((SENSE_PORT >> DOOR_BIT) & 1)
#define SENSE_X1_LIMIT ((SENSE_PORT >> X1_LIMIT_BIT) & 1)
#define SENSE_X2_LIMIT ((SENSE_PORT >> X2_LIMIT_BIT) & 1)
#define SENSE_Y1_LIMIT ((SENSE_PORT >> Y1_LIMIT_BIT) & 1)
#define SENSE_Y2_LIMIT (SENSE_PORT >> Y2_LIMIT_BIT) & 1
#define SENSE_LIMITS ((LIMIT_PORT & LIMIT_MASK) > 0)
#define SENSE_ANY (((LIMIT_PORT & LIMIT_MASK) > 0) && ((SENSE_PORT & SENSE_MASK) > 0))


void control_init();

void control_laser_intensity(uint8_t intensity);  //0-255 is 0-100%

void control_air(bool enable);
void control_gas(bool enable);

// dis/enable if steppers can still 
// move when a limit switch is triggering
void control_limits_overwrite(bool enable);


#endif
