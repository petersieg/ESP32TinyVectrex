#ifndef _HARDWARE_H
 #define _HARDWARE_H

 #include "gbConfig.h"

 //Section Tiny Fast Bitluni
 // VGA output pins  
 #ifdef use_lib_board_wemos
  //WEMOS
  #define PIN_RED_LOW    255
  #define PIN_RED_HIGH   255
  #define PIN_GREEN_LOW  255
  #define PIN_GREEN_HIGH 255
  #define PIN_BLUE_LOW   5
  #define PIN_BLUE_HIGH  255
  #define PIN_HSYNC      23
  #define PIN_VSYNC      17
  //FIN WEMOS
 #else
  //Para FABGL
  #define PIN_RED_LOW    21
  #define PIN_RED_HIGH   22
  #define PIN_GREEN_LOW  18
  #define PIN_GREEN_HIGH 19
  #define PIN_BLUE_LOW   4
  #define PIN_BLUE_HIGH  5
  #define PIN_HSYNC      23
  #define PIN_VSYNC      15 
 #endif 
 //End section Tiny Fast Bitluni

#endif
