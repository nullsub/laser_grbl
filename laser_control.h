/*
  spindle_control.h - spindle control methods
  Part of Grbl

  Copyright (c) 2009-2011 Simen Svale Skogsrud
  Adapted from spindle_control.c by Stephan Hechenberger  

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

#ifndef laser_control_h
#define laser_control_h 

#include "config.h"

#ifdef LASER_MODE 

#include <avr/io.h>

void laser_init();
void laser_enable();
void laser_disable();
void set_laser_intensity(uint8_t intensity);  //0-255 = 0-100%

#endif

#endif
