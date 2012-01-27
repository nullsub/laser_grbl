/*
  limits.h - code pertaining to limit-switches and performing the homing cycle
  Part of LasaurGrbl

  Copyright (c) 2009-2011 Simen Svale Skogsrud
  Copyright (c) 2009-2011 Stefan Hechenberger
  
  LasaurGrbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LasaurGrbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/

#ifndef limits_h
#define limits_h 

#include "mbed.h"


// initialize the limits module
void limits_init();

// perform the homing cycle
void limits_go_home();

// dis/enable if controller can still 
// move while a limit switch is triggering
void limit_overwrite_enable();
void limit_overwrite_disable();


#endif