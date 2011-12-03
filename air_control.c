/*
  air_assist_control.c - air and gas control methods
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

#include "air_assist_control.h"


#include "settings.h"
#include "config.h"
#include <avr/io.h>


void air_assist_init() {
  AIR_ASSIST_DDR |= (1 << DDC4);      // set PC4 as an output ***
  AIR_ASSIST_DDR |= (1 << DDC5);      // set PC5 as an output ***

  // turn off two air assist ports
  air_assist(AIR_OFF);    // Turn off Air1 and Air2
}

void air_assist(uint8_t air_command) {
  // sinking the pin turns on the air assit relay
  // 0 = all air off
  // 1 = AIR1
  // 2 = AIR2
  switch (air_command) {
     case 0: AIR_ASSIST_PORT |= (1 << AIR1_ASSIST_BIT);
             AIR_ASSIST_PORT |= (1 << AIR2_ASSIST_BIT); break;
     case 1: AIR_ASSIST_PORT &= ~(1 << AIR1_ASSIST_BIT); break;
     case 2: AIR_ASSIST_PORT &= ~(1 << AIR2_ASSIST_BIT); break;
  }
}