/*
  airgas_control.c - air and gas assist control
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

#include "airgas_control.h"
#include "settings.h"
#include "config.h"
#include <avr/io.h>


void airgas_init() {
  AIRGAS_DDR |= (1 << AIR_BIT);  // set as output pin
  AIRGAS_DDR |= (1 << GAS_BIT);  // set as output pin
  airgas_disable();    
}


void airgas_disable() {
  AIRGAS_PORT |= (1 << AIR_BIT);
  AIRGAS_PORT |= (1 << GAS_BIT);
}

void air_enable() {
  AIRGAS_PORT &= ~(1 << AIR_BIT);
}

void gas_enable() {
  AIRGAS_PORT &= ~(1 << GAS_BIT);
}

