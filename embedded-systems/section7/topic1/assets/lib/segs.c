#include <hidef.h>      /* common defines and macros */
#include "derivative.h" /* derivative-specific definitions */
#include "segs.h"

unsigned int i = 0; 

void Segs_Init (void) 
{
  // STEP 1 - INITIALIZE PORTA - DOES NOT STROBE
  // PA0 AND PA1 START HIGH 
  PORTA |= 0x03;

  // STEP 2 - SET PA0 AND PA1 AS OUTPUT 
  // ONLY WRITING ON THE 7-SEGS 
  DDRA |= PORTA_PA1_MASK | PORTA_PA0_MASK;

  // STEP 3 - SET ALL BITS ON PORTB AS OUTPUT 
  DDRB |= 0xFF;

  // STEP 4 - Clear all of the Segs
  Segs_Clear(); 
}

// show HEX decoding or CUSTOM decoding (w/wo dp)
void Segs_Display (unsigned char addr, unsigned char data, Segs_ModeOption mode, Segs_DPOption dp)
{
  //Need to present data on PORTB - DATASHEET FOR THE CONTROLLER (NOTES)
if(mode == Segs_Mode_Hex){
  PORTB = 0b01011000;
/*          ||||||||_____BIT 0-2 , address
            |||||________BIT3, always use BANK A (1)
            ||||_________BIT4, always NORMAL (1)
            |||__________BIT5, Decode (0) as here, No-Decode would be (1)
            ||___________BIT6, Decode as HEX (1), Irrelevant if No-Decode
            |____________BIT7, no data coming (0)
*/
}
else{
  PORTB = 0b01111000;
  /*          ||||||||_____BIT 0-2 , address
              |||||________BIT3, always use BANK A (1)
              ||||_________BIT4, always NORMAL (1)
              |||__________BIT5, No-Decode (1) as here, Decode would be (0)
              ||___________BIT6, Irrelevant if No-Decode
              |____________BIT7, no data coming (0)
  */
}
  addr &= 0x7; //0b00000111, mask address to 3 bits
  PORTB |= addr;  

  // Select CONTROL BYTE
  // PA1 = 1
  PORTA |= PORTA_PA1_MASK; 

  // STROBE - DRIVE PA0 LOW FOR CERTAIN PERIOD OF TIME THEN HIGH
  // PA0 = 0 - Load into Memory
  PORTA &= ~PORTA_PA0_MASK;
  // Add delay if bus speed is faster than 8MHZ 
  // PA0 = 1 - Latch Control Byte     
  PORTA |= PORTA_PA0_MASK; 

  if(dp == Segs_DP_ON){
    // AND data with 0b01111111 to turn ON
    // dp (decimal point), MSB = 0,
    // without affecting other data bits.
    data &= ~SEG_DP;
  }
  else{
    // OR data with 0b10000000 to turn OFF
    // dp (decimal point), MSB = 1,
    // without affecting other data bits.
    data |= SEG_DP;
  }

  // Send Data Byte
  PORTB = data;

  // Select DATA BYTE
  // PA1 = 0
  PORTA &= ~PORTA_PA1_MASK;

  // STROBE - DRIVE PA0 LOW FOR CERTAIN PERIOD OF TIME THEN HIGH
  // STROBE IS THE SAME FOR DATA BYTE AND COMMAND BYTE 
  // PA0 = 0 - Load into Memory
  PORTA &= ~PORTA_PA0_MASK;
  // Add delay if bus speed is faster than 8MHZ  
  // PA0 = 1 - Latch Control Byte
  PORTA |= PORTA_PA0_MASK;
}

// clear a digit
void Segs_ClearDigit(unsigned char addr)
{
  Segs_Display (addr, 0x80, Segs_Mode_Custom, Segs_DP_OFF); 
}

// clear the entire display
void Segs_Clear(void)
{
  for(i=0;i<8;i++)
  {
    Segs_ClearDigit(i); 
  }
}

// clear display  upper or lower line
void Segs_ClearLine(Segs_LineOption line)
{
  if(line)
  {
    for(i=4;i<8;i++)
    {
      Segs_ClearDigit(i); 
    }
  } 
  else 
  {
    for(i=0;i<4;i++)
    {
      Segs_ClearDigit(i); 
    }
  }
}

// show a 16-bit value on the upper or lower display
void Segs_16H (unsigned int value, Segs_LineOption line)
{
  unsigned int mask = 0xF000;
  unsigned int number;
  unsigned int addr;  
  
  if(line)
  {
    addr = 4; 
    for(i=0;i<4;i++)
    {
      number = value & mask; 
      number>>= (12-4*i);
      mask>>=4;
      Segs_Display(addr, (unsigned char)number, Segs_Mode_Hex, Segs_DP_OFF); 
      ++addr; 
    }
  } 
  else
  {
    addr = 0; 
    for(i=0;i<4;i++)
    {
      number = value & mask; 
      number>>= (12-4*i);
      mask>>=4;
      Segs_Display(addr, (unsigned char)number, Segs_Mode_Hex, Segs_DP_OFF); 
      ++addr; 
    }
  }
  
}

// show the 8-bit value starting on the digit as addr (0-6)
void Segs_8H (unsigned char addr, unsigned char value)
{
  unsigned char mask = 0xF0; 
  unsigned char number; 
  
  for(i=0;i<2;i++)
  {
    number = value & mask; 
    number>>=(4-4*i); 
    mask >>=4; 
    Segs_Display(addr,(unsigned char) number, Segs_Mode_Hex, Segs_DP_OFF); 
    if(addr == 7)
      addr = 0; 
    else 
      ++addr; 
  }
}

// show a decimal value on the first or second line of the 7-segs
void Segs_16D (unsigned int value, Segs_LineOption line)
{
   unsigned int addr;
   unsigned int power = 1; 
   unsigned int number;  
   
   if(line)
   {
      addr = 7; 
      for(i=0;i<4;i++)
      {
        number = (value/power)%10; 
        Segs_Display(addr--,(unsigned char)number, Segs_Mode_Hex, Segs_DP_OFF);
        power*=10; 
      }
   }
   else
   {
      addr = 3; 
      for(i=0;i<4;i++)
      {
        number = (value/power)%10; 
        Segs_Display(addr--,(unsigned char)number, Segs_Mode_Hex, Segs_DP_OFF);
        power*=10; 
      }
   }
}

// say Err on the appropriate line
void Segs_SayErr (Segs_LineOption line)
{
  if(line)
  {
    Segs_Display(4, SEG_A|SEG_F|SEG_G|SEG_E|SEG_D, Segs_Mode_Custom, Segs_DP_OFF); 
    Segs_Display(5, SEG_E | SEG_G, Segs_Mode_Custom, Segs_DP_OFF); 
    Segs_Display(6,SEG_E | SEG_G, Segs_Mode_Custom, Segs_DP_OFF); 
    Segs_ClearDigit(7); 
  } 
  else
  {
    Segs_Display(0, SEG_A|SEG_F|SEG_G|SEG_E|SEG_D, Segs_Mode_Custom, Segs_DP_OFF); 
    Segs_Display(1, SEG_E | SEG_G, Segs_Mode_Custom, Segs_DP_OFF); 
    Segs_Display(2,SEG_E | SEG_G, Segs_Mode_Custom, Segs_DP_OFF); 
    Segs_ClearDigit(3); 
  }
}
