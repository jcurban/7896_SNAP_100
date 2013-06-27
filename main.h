/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __MAIN_h
#define __MAIN_h

  /*set temp limits in degrees F */
  #define TUpLimF  125
  #define TLoLimF  50


/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_flash.h"
#include "stm8s_conf.h"

/* Public variables ----------------------------------------------------------*/
extern  u8 Mode1;   /* bit 0 = 1(DegF),0(DegC)*/
extern  u8  SysError; /* bit 0 = 1 if Tempreading out of limits*/
                      /*bit 1 = 1 if I cannot control the steam generator*/
/* INSERT VERSION NUMBER FOR THIS RELEASE.*/
  #define VER0  0   /*MSD*/
  #define VER1  2
  #define VER2  0
  #define VER3  4  /*VERSION 2.04*/
#endif

  /*CUSTOMER REQUESTED OFFSET*/
  #define TempOffsetC -1.11 /*OFFSET TEMP IN DEG C FOR TEMP READING*/
                            /*1.8 DEG F FOR EACH DEGEE C*/
