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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  ---
  Some Notes about setting up GPIO pins via registers:
  The relevant registers are the following:
  LPC_PINCON->PINSEL0
  LPC_PINCON->PINMODE0
  LPC_GPIO0->FIODIR
  LPC_GPIO0->FIOMASK
  (LPC_GPIO0 for Port0, LPC_GPIO1 for Port1, ...)
  
  By default the PINSELx is set to the primary function on the pin which
  usually is GPIO (as opposed to serial or I2C). PINMODEx is set to
  have a pull-up resistor enabled. To disable any pull-up or pull-down
  resistors: LPC_PINCON->PINSELx &= ~(2UL << 2*pinnum);
  
  To control the pin use any of:
  LPC_GPIOx->FIOPIN
  LPC_GPIOx->FIOSET
  LPC_GPIOx->FIOCLR
*/

#include "more_control.h"


PwmOut laserPWM(LASER_PWM_PIN);
PwmOut led3PWM(LED3);
DigitalOut led4(LED4);


void laser_init() {
  // PWMs can be set up on p21-p26 (p23-26 is also LED1-LED4)
  // http://mbed.org/handbook/PwmOut    
  laserPWM.period(0.020);             // 20ms period/50KHz, is the default
  laserPWM.write(0.0);                // default
  led4 = 0;
}


void set_laser_intensity(float intensity) {
  laserPWM.write(intensity);  // set pwm as a 0.0-1.0 percentage
  led3PWM.write(intensity);
}



void airgas_init() {
  AIRGAS_PORT->FIODIR |= (1 << AIR_BIT);  // set as output pin
  AIRGAS_PORT->FIODIR |= (1 << GAS_BIT);  // set as output pin
  airgas_disable();    
}


void airgas_disable() {
  AIRGAS_PORT->FIOCLR = (1 << AIR_BIT);
  AIRGAS_PORT->FIOCLR = (1 << GAS_BIT);
  led4 = 0;
}

void air_enable() {
  AIRGAS_PORT->FIOSET = (1 << AIR_BIT);
  led4 = 1;
}

void gas_enable() {
  AIRGAS_PORT->FIOSET = (1 << GAS_BIT);
  led4 = 1;
}

