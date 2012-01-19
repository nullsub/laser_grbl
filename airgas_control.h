/*
  airgas_control.h - air and gas assist control
  Part of LasaurGrbl

  Copyright (c) 2011 Stephan Hechenberger

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

#ifndef airgas_control_h
#define airgas_control_h 

#include "config.h"
#include <avr/io.h>

void airgas_init();
void airgas_disable();
void air_enable();
void gas_enable();

#endif
