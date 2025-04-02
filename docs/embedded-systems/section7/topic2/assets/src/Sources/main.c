/********************************************************************/
// HC12 Program:  ICA09 Segs
// Processor:     MC9S12XDP512
// Bus Speed:     20 MHz
// Author:        Robbin Law
// Details:       
//                
// Date:          2025-03-29
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
#define TIER3
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
unsigned int counterUp = 0, counterDown = 0xffff;
unsigned char myArray[6] = {SEG_A,SEG_B,SEG_C,SEG_D,SEG_E,SEG_F};
char iX = 0, index = 0;
unsigned int changeCounter  = 0;
int loopCounter = 0;

/********************************************************************/
// Constants
/********************************************************************/

/********************************************************************/
// Main Entry
/********************************************************************/
void main(void)
{
  //Any main local variables must be declared here
  unsigned char flipped7 = SEG_A | SEG_F | SEG_E;
  unsigned char normal7 = SEG_A | SEG_B | SEG_C;
  unsigned char normalL = SEG_F | SEG_E | SEG_D;
  unsigned char flippedL = SEG_B | SEG_C | SEG_D;
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

  #ifdef TIER1
    Segs_Display(0, 0x4, Segs_Mode_Hex, Segs_DP_OFF);
    Segs_Display(1, flipped7, Segs_Mode_Custom, Segs_DP_ON);
    Segs_Display(2, normal7, Segs_Mode_Custom, Segs_DP_OFF);
    Segs_Display(4, 0x3, Segs_Mode_Hex, Segs_DP_OFF);
    Segs_Display(5, normalL, Segs_Mode_Custom, Segs_DP_OFF);
    Segs_Display(6, flippedL, Segs_Mode_Custom, Segs_DP_OFF);
    Segs_Display(7, 0xE, Segs_Mode_Hex, Segs_DP_OFF);
  #endif 
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

void RunTier1(void){
  #ifdef TIER1
    counterUp = 0;
    counterDown = 0xFFFF;
    for (;;)
    { 
      RTI_Delay_ms(100);
      Segs_16H(counterUp++, Segs_LineTop);
      Segs_16H(counterDown--, Segs_LineBottom);
      if(Sw_State(&centerState, SWL_CTR) == Pressed){
        break;
      }
    }
  #endif
}

// In this Tier you must hold a button press
// for a period of time before anything happens.
void RunTier2(void){
  #ifdef TIER2
    loopCounter = 0;
    changeCounter = 0;
    iX = 0;
    index = 0;
    Segs_Clear();
    for (;;)
    {
      //Iterate through the animation array
      Segs_ClearLine(Segs_LineTop); 
      Segs_Display(iX, myArray[index], Segs_Mode_Custom, Segs_DP_OFF);
      RTI_Delay_ms(100);   
      if(++index > 5)
      {
        index = 0;
      }      

      if(++loopCounter > 3)
      {
        loopCounter = 0;
        if(Sw_State(&leftState, SWL_LEFT) == Pressed)
        {
          // cap the display index (0-3)
          if(--iX < 0)
          {
            iX = 0;
          }
          else
          {
            ++changeCounter;
            Segs_16D(changeCounter, Segs_LineBottom);     
          }           
        } 
        if(Sw_State(&rightState, SWL_RIGHT) == Pressed)
        {
          // cap the display index (0-3)         
          if(++iX > 3)
          {
            iX = 3;
          } 
          else
          {
            ++changeCounter;
            Segs_16D(changeCounter, Segs_LineBottom);     
          }         
        } 
      }      
      if(Sw_State(&centerState, SWL_CTR) == Pressed){
        break;
      }
    }
  #endif
}

// In this Tier3 you must press a button
// 4 times before anything happens.
void RunTier3(void){
  #ifdef TIER3
    loopCounter = 0;
    changeCounter = 0;
    iX = 0;
    index = 0;
    Segs_Clear();
    for (;;)
    { 
      //Iterate through the animation array
      Segs_ClearLine(Segs_LineTop);
      Segs_Display(iX, myArray[index], Segs_Mode_Custom, Segs_DP_OFF);
      RTI_Delay_ms(100);   
      if(++index > 5)
      {
        index = 0;
      }     
      if(Sw_State(&leftState, SWL_LEFT) == Pressed){
        // Increment counter and check if it's > 3
        if(++loopCounter > 3)
        {
          loopCounter = 0;
          // cap the display index (0-3)
          if(--iX < 0){
            iX = 0;
          }
          else{
            ++changeCounter;
            Segs_16D(changeCounter, Segs_LineBottom);     
          }             
        }        
      }
      if(Sw_State(&rightState, SWL_RIGHT) == Pressed){
        // Increment counter and check if it's > 3
        if(++loopCounter > 3)
        {    
          loopCounter = 0;
          // cap the display index (0-3)
          if(++iX > 3){
            iX = 3;
          } 
          else{
            ++changeCounter;
            Segs_16D(changeCounter, Segs_LineBottom); 
          }             
        }              
      }     
      if(Sw_State(&centerState, SWL_CTR) == Pressed){
        break;
      }
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