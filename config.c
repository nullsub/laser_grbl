/*
config.c - compile-time configuration and dynamic settings
Part of LasaurGrbl

Copyright (c) 2009-2011 Simen Svale Skogsrud
Copyright (c) 2011 Stefan Hechenberger
Copyright (c) 2011 Sungeun K. Jeon

LasaurGrbl is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LasaurGrbl is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#include "config.h"

settings_t settings;

void settings_init() {
  settings.steps_per_mm[X_AXIS] = DEFAULT_X_STEPS_PER_MM;
  settings.steps_per_mm[Y_AXIS] = DEFAULT_Y_STEPS_PER_MM;
  settings.steps_per_mm[Z_AXIS] = DEFAULT_Z_STEPS_PER_MM;
  settings.pulse_microseconds = DEFAULT_STEP_PULSE_MICROSECONDS;
  settings.default_feed_rate = DEFAULT_FEEDRATE;
  settings.default_seek_rate = DEFAULT_RAPID_FEEDRATE;
  settings.acceleration = DEFAULT_ACCELERATION*60*60; // convert to mm/min^2 for internal use
  settings.mm_per_arc_segment = DEFAULT_MM_PER_ARC_SEGMENT;
  settings.junction_deviation = DEFAULT_JUNCTION_DEVIATION;
  settings.invert_mask = 0x00000000;
  if (X_DIRECTION_INV) {
    settings.invert_mask |= (1<<X_DIRECTION_BIT);
  }
  if (Y_DIRECTION_INV) {
    settings.invert_mask |= (1<<Y_DIRECTION_BIT);
  }
  if (Z_DIRECTION_INV) {
    settings.invert_mask |= (1<<Z_DIRECTION_BIT);
  }
}