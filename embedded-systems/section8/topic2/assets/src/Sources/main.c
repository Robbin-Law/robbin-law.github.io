/********************************************************************/
// HC12 Program:  ICA10 lcd
// Processor:     MC9S12XDP512
// Bus Speed:     20 MHz
// Author:        Robbin Law               
// Date:          2025-04-05
// Revision History :

/********************************************************************/
// Library includes
/********************************************************************/
// Common defines and macros
#include <hidef.h>
// Derivative-specific definitions
#include "derivative.h"
#include "sw_led.h"
#include "clock.h"
#include "rti.h"
#include "sci.h"
#include "segs.h"
#include "lcd.h"
// Other system includes or your includes go here
//#include <stdlib.h>
// For using sscanf and sprintf
#include <stdio.h>
// For using memset and memcpy
#include <string.h>
/********************************************************************/
//Defines
/********************************************************************/
#define TIER1
#define TIER2
//#define TIER3
/********************************************************************/
// Local Prototypes
/********************************************************************/
void Part1(void);
void RunTier1(void);
void RunTier2(void);
void RunTier3(void);

/********************************************************************/
// Global Variables
/********************************************************************/
SwState leftState, centerState, rightState, upState, downState = Idle;

/********************************************************************/
// Constants
/********************************************************************/

/********************************************************************/
// Main Entry
/********************************************************************/
void main(void)
{
  //Any main local variables must be declared here
  
  // main entry point
  _DISABLE_COP();
  EnableInterrupts;
/********************************************************************/
  // one-time initializations
/********************************************************************/
  SWL_Init();
  Clock_Set20MHZ();
  RTI_Init();
  SCI_Init(9600);
  Segs_Init();
  lcd_Init ();
/********************************************************************/
  // main program loop
/********************************************************************/
  // Part1 demonstrates...
  Part1();
}
/********************************************************************/
// Functions
/********************************************************************/

void Part1(void){
  for (;;)
  {
    if(Sw_State(&leftState, SWL_LEFT) == Pressed)
    {
      RunTier1(); 
    }   
    if(Sw_State(&upState, SWL_UP) == Pressed)
    {
      RunTier2(); 
    }
    if(Sw_State(&rightState, SWL_RIGHT) == Pressed)
    {
      RunTier3(); 
    }
  }     
}

// In this Tier1 we print characters to the lcd.
void RunTier1(void){
  #ifdef TIER1
    unsigned char mychar = 'T';
    for (;;)
    { 
      RTI_Delay_ms(500);
      PTH = mychar;
      LCD_WRITE
      LCD_DATA
      LCD_LATCH

      if(Sw_State(&leftState, SWL_LEFT) == Pressed)
        mychar = 'D';   
      if(Sw_State(&upState, SWL_UP) == Pressed)
        mychar = 'E';
      if(Sw_State(&rightState, SWL_RIGHT) == Pressed)
        mychar = 'F';
        if(Sw_State(&downState, SWL_DOWN) == Pressed)
        mychar = 'G';
      if(Sw_State(&centerState, SWL_CTR) == Pressed){
        lcd_Clear();
        lcd_Home();
        break;
      }
    }
  #endif
}

// In this Tier2 we move the cursor around the lcd screen.
void RunTier2(void){
  #ifdef TIER2
    unsigned char cursor_position = 0;
    unsigned char X = 0;
    unsigned char Y = 0;
    lcd_DispControl(1,1);
    lcd_StringXY(3,1,"Press Buttons");
    lcd_StringXY(3,2,"To Move Cursor");
    lcd_Home();
    for (;;)
    {
      Segs_16D(lcd_GetAddr(), Segs_LineTop);
      
      if(Sw_State(&leftState, SWL_LEFT) == Pressed){
        lcd_AddrXY(X-=1,Y);
      }
      if(Sw_State(&upState, SWL_UP) == Pressed){
        lcd_AddrXY(X, Y-=1);
      }
      if(Sw_State(&rightState, SWL_RIGHT) == Pressed){
        lcd_AddrXY(X+=1,Y);
      }
      if(Sw_State(&downState, SWL_DOWN) == Pressed){
        lcd_AddrXY(X, Y+=1);
      }
      if(Sw_State(&centerState, SWL_CTR) == Pressed){
        lcd_Clear();
        lcd_Home();
        Segs_Clear();
        break;
      }
    }
  #endif
}

// In this Tier3 
void RunTier3(void){
  #ifdef TIER3
    for (;;)
    {

    }
  #endif
}

/********************************************************************/
// Interrupt Service Routines
/********************************************************************/

interrupt VectorNumber_Vrti void Vrti_ISR(void)
{
  CRGFLG = CRGFLG_RTIF_MASK;
  rtiMasterCount++;
}