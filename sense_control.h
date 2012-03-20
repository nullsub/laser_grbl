/*
  sense_control.h - sensing and controlling inputs and outputs
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

#ifndef sense_control_h
#define sense_control_h

#include <stdbool.h>


void sense_init();
#define SENSE_POWER_OFF !((SENSE_PIN >> POWER_BIT) & 1)
#define SENSE_CHILLER_OFF !((SENSE_PIN >> CHILLER_BIT) & 1)
#define SENSE_DOOR_OPEN !((SENSE_PIN >> DOOR_BIT) & 1)
#define SENSE_X1_LIMIT !((LIMIT_PIN >> X1_LIMIT_BIT) & 1)
#define SENSE_X2_LIMIT !((LIMIT_PIN >> X2_LIMIT_BIT) & 1)
#define SENSE_Y1_LIMIT !((LIMIT_PIN >> Y1_LIMIT_BIT) & 1)
#define SENSE_Y2_LIMIT !((LIMIT_PIN >> Y2_LIMIT_BIT) & 1)
#define SENSE_LIMITS (SENSE_X1_LIMIT || SENSE_X2_LIMIT || SENSE_Y1_LIMIT || SENSE_Y2_LIMIT)
#define SENSE_ANY (SENSE_LIMITS || SENSE_POWER_OFF || SENSE_CHILLER_OFF || SENSE_DOOR_OPEN)


void control_init();

void control_laser_enable(bool enable);
void control_laser_intensity(uint8_t intensity);  //0-255 is 0-100%

void control_air(bool enable);
void control_gas(bool enable);

// dis/enable if steppers can still 
// move when a limit switch is triggering
void control_limits_overwrite(bool enable);


#endif
