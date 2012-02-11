/*
  motion_control.c - high level interface for issuing motion commands
  Part of Grbl

  Copyright (c) 2009-2011 Simen Svale Skogsrud
  Copyright (c) 2011 Sungeun K. Jeon
  Copyright (c) 2011 Stefan hechenberger  
  
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

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
#include "config.h"
#include "input_control.h"
#include "output_control.h"
#include "stepper.h"
#include "planner.h"

// Execute dwell in seconds. Maximum time delay is > 18 hours, more than enough for any application.
void mc_dwell(double seconds) {
   uint16_t i = floor(seconds);
   st_synchronize();
   _delay_ms(floor(1000*(seconds-i))); // Delay millisecond remainder
   while (i > 0) {
     _delay_ms(1000); // Delay one second
     i--;
   }
}





void laser_init() {
  // Setup Timer0 for a 31.25kH "phase correct PWM" wave (assuming a 16Mhz clock)
  // Timer0 can pwm either PD5 (OC0B) or PD6 (OC0A), we use PD6
  // TCCR0A and TCCR0B are the registers to setup Timer0
  // see chapter "8-bit Timer/Counter0 with PWM" in Atmga328 specs
  // OCR0A sets the duty cycle 0-255 corresponding to 0-100%
  DDRD |= (1 << DDD6);      // set PD6 as an output
  OCR0A = 0;              // set PWM to a 0% duty cycle
  TCCR0A |= (1 << COM0A1);  // set non-inverting mode on OC0A, PD6, Arduino pin 6
  TCCR0A |= (1 << WGM00);   // set phase correct PWM mode, has half the freq of fast PWM
  TCCR0B |= (1 << CS00);    // prescaler to 1, PWMfreq = 16000/(2*256*1) = 31.25kH
  
  // Setup laser enable pin.
  // LASER_ENABLE_DDR |= (1 << LASER_ENABLE_BIT);
  // laser_enable();
}

void laser_enable() {
  // sinking the pin enables the laser
  // LASER_ENABLE_PORT &= ~(1<<LASER_ENABLE_BIT);
}

void laser_disable() {
  // LASER_ENABLE_PORT |= 1<<LASER_ENABLE_BIT;
}

void set_laser_intensity(uint8_t intensity) {
  OCR0A = intensity;
}

void set_laser_intensity(float intensity) {
  OCR0A = (uint8_t)(intensity*255);
}


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


