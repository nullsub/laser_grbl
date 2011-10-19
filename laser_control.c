/*
  laser_control.c - laser control methods
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

#include "laser_control.h"

#ifdef LASER_MODE     

#include "settings.h"
#include "stepper.h"
#include "config.h"
#include <avr/io.h>


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

#endif

