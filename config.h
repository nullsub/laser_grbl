/*
  config.h - compile-time configuration and dynamic settings
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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/

#ifndef config_h
#define config_h

#include "mbed.h"


#define BAUD_RATE 9600
#define LASER_OFF 0


#define SDCARD_PORT             LPC_GPIO0  //Port0
#define MOSI_SD_BIT             9          //p5
#define MISO_SD_BIT             8          //p6
#define SCLK_SD_BIT             7          //p7
#define CS_SD_BIT               6          //p8

#define STEPPING_PORT           LPC_GPIO0  //Port0
#define X_STEP_BIT              0          //p9
#define X_DIRECTION_BIT         1          //p10
#define Y_STEP_BIT              18         //p11
#define Y_DIRECTION_BIT         17         //p12
#define Z_STEP_BIT              15         //p13
#define Z_DIRECTION_BIT         16         //p14

#define X_DIRECTION_INV         true       //inverted
#define Y_DIRECTION_INV         true       //inverted
#define Z_DIRECTION_INV         true       //inverted

#define LIMIT_XY_PORT           LPC_GPIO0  //Port0  
#define X1_LIMIT_BIT            23         //p15
#define X2_LIMIT_BIT            24         //p16
#define Y1_LIMIT_BIT            25         //p17
#define Y2_LIMIT_BIT            26         //p18

#define LIMIT_Z_PORT            LPC_GPIO1  //Port1  
#define Z1_LIMIT_BIT            30         //p19
#define Z2_LIMIT_BIT            31         //p20

#define LIMIT_OVERWRITE_PORT    LPC_GPIO2  //Port2  
#define LIMIT_OVERWRITE_BIT     5          //p21

#define SENSE_PORT              LPC_GPIO2  //Port2
#define POWER_BIT               4          //p22
#define CHILLER_BIT             3          //p23
#define DOOR_BIT                2          //p24
//#define STILL_AVAILABLE       1          //p25

#define LASER_PORT              LPC_GPIO2  //Port2
#define LASER_PWM_BIT           0          //p26
#define LASER_PWM_PIN           p26        //pwm possible on p21-26

#define AIRGAS_PORT             LPC_GPIO0  //Port0
#define AIR_BIT                 11         //p27
#define GAS_BIT                 10         //p28

#define ETHERNET_LED_PORT       LPC_GPIO0  //Port0
#define ETHERNET_GREEN_BIT      5          //p29
#define ETHERNET_YELLOW_BIT     4          //p30


#define LIMIT_MASK ((1<<X1_LIMIT_BIT)|(1<<X2_LIMIT_BIT)|(1<<Y1_LIMIT_BIT)|(1<<Y2_LIMIT_BIT)) // All limit bits
#define STEP_MASK ((1<<X_STEP_BIT)|(1<<Y_STEP_BIT)|(1<<Z_STEP_BIT)) // All step bits
#define DIRECTION_MASK ((1<<X_DIRECTION_BIT)|(1<<Y_DIRECTION_BIT)|(1<<Z_DIRECTION_BIT)) // All direction bits


// bit math
// see: http://www.arduino.cc/playground/Code/BitMath
// see: http://graphics.stanford.edu/~seander/bithacks.html
//
// y = (x >> n) & 1;    // n=0..15.  stores nth bit of x in y.  y becomes 0 or 1.
// 
// x &= ~(1 << n);      // forces nth bit of x to be 0.  all other bits left alone.
// 
// x &= (1<<(n+1))-1;   // leaves alone the lowest n bits of x; all higher bits set to 0.
// 
// x |= (1 << n);       // forces nth bit of x to be 1.  all other bits left alone.
// 
// x ^= (1 << n);       // toggles nth bit of x.  all other bits left alone.
// 
// x = ~x;              // toggles ALL the bits in x.



// example
// PortOut limit_overwrite_port(Port0, 0x800000)

// http://mbed.org/handbook/PortIn
// http://mbed.org/handbook/PortOut
// http://mbed.org/users/Lerche/notebook/lpc1768-pin-functions/


// pin configuration from PinNames.h
//
// , p5 = P0_9 
// , p6 = P0_8
// , p7 = P0_7
// , p8 = P0_6
// , p9 = P0_0
// , p10 = P0_1
// , p11 = P0_18
// , p12 = P0_17
// , p13 = P0_15
// , p14 = P0_16
// , p15 = P0_23
// , p16 = P0_24
// , p17 = P0_25
// , p18 = P0_26
// 
// , p19 = P1_30
// , p20 = P1_31
//
// , p21 = P2_5
// , p22 = P2_4
// , p23 = P2_3
// , p24 = P2_2
// , p25 = P2_1
// , p26 = P2_0
//
// , p27 = P0_11
// , p28 = P0_10
// , p29 = P0_5
// , p30 = P0_4
// 
// // Other mbed Pin Names
// , LED1 = P1_18
// , LED2 = P1_20
// , LED3 = P1_21
// , LED4 = P1_23
// , USBTX = P0_2
// , USBRX = P0_3
//
// P0 ... LPC_GPIO0
// P1 ... LPC_GPIO1
// P2 ... LPC_GPIO2
// P3 ... LPC_GPIO3
// P4 ... LPC_GPIO4


// Port0 - examples
// LPC_GPIO0->FIODIR   ... set pin as output/input for 1s/0s
// LPC_GPIO0->FIOMASK  ... any pin marked with 1 is disabled from read/writes
// LPC_GPIO0->FIOPIN   ... set pins high/low for 1s/0s
// LPC_GPIO0->FIOSET   ... set pins high for any 1s
// LPC_GPIO0->FIOCLR   ... sets pins low for any 1s
// see: p122 in lpc17xx manual
// 
// #include "mbed.h"
// int main() {
//   int dir = 0;
//   LPC_GPIO0->FIODIR = dir;
//   LPC_GPIO0->FIODIR = 0x07800000 | dir;
//   LPC_GPIO0->FIOMASK = 0xF87FFFFF;
//   while(1){
//     LPC_GPIO0->FIOSET = 0x07800000;
//     LPC_GPIO0->FIOCLR = 0x07800000;
//   }
// }
//
// void write(int value) {
//     _gpio->FIOPIN = (_gpio->FIOPIN & ~_mask) | (value & _mask);
// }    
//    
// from: http://mbed.org/users/igorsk/notebook/fast-gpio-with-c-templates/


#define MICROSTEPS                       10
#define DEFAULT_X_STEPS_PER_MM           32.80839895    //microsteps/mm
#define DEFAULT_Y_STEPS_PER_MM           32.80839895    //microsteps/mm
#define DEFAULT_Z_STEPS_PER_MM           32.80839895    //microsteps/mm
#define DEFAULT_STEP_PULSE_MICROSECONDS  5
#define DEFAULT_MM_PER_ARC_SEGMENT       0.1
#define DEFAULT_RAPID_FEEDRATE           20000.0        // in millimeters per minute
#define DEFAULT_FEEDRATE                 5000.0
#define DEFAULT_ACCELERATION             500.0          // mm/sec^2
#define DEFAULT_JUNCTION_DEVIATION       0.1            // mm


#define LASAURGRBL_VERSION "v11.12"


// The temporal resolution of the acceleration management subsystem. Higher number give smoother
// acceleration but may impact performance.
// NOTE: Increasing this parameter will help any resolution related issues, especially with machines 
// requiring very high accelerations and/or very fast feedrates. In general, this will reduce the 
// error between how the planner plans the motions and how the stepper program actually performs them.
// However, at some point, the resolution can be high enough, where the errors related to numerical 
// round-off can be great enough to cause problems and/or it's too fast for the Arduino. The correct
// value for this parameter is machine dependent, so it's advised to set this only as high as needed.
// Approximate successful values can range from 30L to 100L or more.
#define ACCELERATION_TICKS_PER_SECOND 100L

// Minimum planner junction speed. Sets the default minimum speed the planner plans for at the end
// of the buffer and all stops. This should not be much greater than zero and should only be changed
// if unwanted behavior is observed on a user's machine when running at very slow speeds.
#define ZERO_SPEED 0.0 // (mm/min)

// Minimum stepper rate. Sets the absolute minimum stepper rate in the stepper program and never runs
// slower than this value, except when sleeping. This parameter overrides the minimum planner speed.
// This is primarily used to guarantee that the end of a movement is always reached and not stop to
// never reach its target. This parameter should always be greater than zero.
#define MINIMUM_STEPS_PER_MINUTE 1600 // (steps/min) - Integer value only
// 1600 @ 32step_per_mm = 50mm/min

// Number of arc generation iterations by small angle approximation before exact arc trajectory 
// correction. This parameter maybe decreased if there are issues with the accuracy of the arc
// generations. In general, the default value is more than enough for the intended CNC applications
// of grbl, and should be on the order or greater than the size of the buffer to help with the 
// computational efficiency of generating arcs.
#define N_ARC_CORRECTION 25 // Integer (1-255)



typedef struct {
    double steps_per_mm[3];
    uint32_t microsteps;
    uint32_t pulse_microseconds;
    double default_feed_rate;
    double default_seek_rate;
    double mm_per_arc_segment;
    double acceleration;
    double junction_deviation;
    uint32_t invert_mask;
} settings_t;
extern settings_t settings;


void settings_init();


#define false 0
#define true 1

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

#define clear_vector(a) memset(a, 0, sizeof(a))
#define clear_vector_double(a) memset(a, 0.0, sizeof(a))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))


#endif

