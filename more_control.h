/*
more_control.h - control of various subsystems
Part of LasaurGrbl

Copyright (c) 2011 Stephan Hechenberger

LasaurGrbl is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LasaurGrbl is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#ifndef more_control_h
#define more_control_h

#include "config.h"
#include <avr/io.h>


void laser_init();
void set_laser_intensity(uint8_t intensity);  //0-255
void set_laser_intensity(float intensity);    //0.0-1.0 percentage


void airgas_init();
void airgas_disable();
void air_enable();
void gas_enable();

#endif