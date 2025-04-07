// liquid Crystal Display
// Robbin Law
// 2025-04-04

#include <hidef.h>
#include "derivative.h"

#include "lcd.h"
#include "lcd_custom_characters.h"
#include "rti.h"

void lcd_Init (void){
	// PH7-PH0 -> D7 - D0 (data Lines)
	DDRH = 0xFF;
	// PK2-PK0 -> Control Lines (output)
	// Bits 0 - 2 of port K must be outputs; 
	// they are the LCD control lines
	DDRK |= 0x07;
	// Wait at least 45ms after power up (Datasheet pg. 45)
	RTI_Delay_ms(45);
	// Set RS for instructions not data
	LCD_INST
	// Set R/W and Port H for writing to LCD controller
	LCD_WRITE
	// Put data on Port H (Datasheet pg. 45, 24, 27)
	PTH = 0b00111000;
/*               |_____(don't care)
                ||_____(don't care)
               |||_____font:  5x7 matrix (LOW)
              ||||_____lines: 2 (HIGH)
             |||||_____data:  8-bit (HIGH)
            ||||||_____function set of commands (HIGH)
*/
	// Latch data into LCD controller (1st time)
	LCD_LATCH
	// Wait at least 4.1 ms (Datasheet pg. 45)
	RTI_Delay_ms(5);
	//Latch in the same data as before (2nd time)
	LCD_LATCH
	//Wait at least 100us (Datasheet pg. 45)
	RTI_Delay_ms(1);
	//Lath in the same data as before (3rd time)
	LCD_LATCH
	
	// The LCD busy flag can now be read 
	// rather than using delays (Datasheet pg. 45)

	// Set the LCD for cursor on, no blinking
	lcd_DispControl(1, 0);
	// Clear the LCD
	lcd_Clear();
	// Move cursor to home position.
	lcd_Home();
	// Set entry mode increment cursor (move left), no display shift
	lcd_Ins(0b00000110);
/*                 |______no display shift (LOW)
                  ||______sets cursor move direction LEFT (HIGH)
								 |||______entry mode set of commands (HIGH)
*/
}

char lcd_Busy (void){
	// Set RS for instruction (Datasheet pg. 24)
	LCD_INST
	// Set up micro to read from LCD controller
	LCD_READ
	// Latch the read command
	LCD_LATCH
	// Return 1 if LCD is busy, 
	// 0 if it is ready for more instructions
	return (PTH & PTH_PTH7_MASK)?1:0;
}

void lcd_Ins (unsigned char instruction){
	// Wait until LCD is not busy
	while(lcd_Busy());
	// Set up port H and R/W for writing
	LCD_WRITE
	// Put the instruction on port H
	PTH = instruction;
	// Latch the instruction
	LCD_LATCH
}

void lcd_Clear (void){
	lcd_Ins(0b00000001);
/*                 |_______clears entire display (HIGH)
*/
}

void lcd_Home (void){
	lcd_Ins(0b00000010);
/*                 |_______(don't care)
                  ||_______returns cursor to home position (HIGH)
*/
}

void lcd_DispControl (unsigned char cursoron, unsigned char blinkon){
	// Start with the display control instruction and the LCD turned on
	unsigned char control;
	control = 0b00001100;
/*                   |_______cursor blink OFF (LOW)
                    ||________cursor OFF (LOW)
                   |||_________entire display ON (HIGH)
                  ||||__________display set of commands (HIGH)
*/
	// Set the curser on and blink options
	control |= (cursoron<<1)|blinkon;      
	lcd_Ins(control);
}

char lcd_GetAddr(void){
	LCD_INST //Set up RS for instruction
	LCD_READ //Set up RW and port H for reading
	LCD_LATCH //Latch command
	return (PTH & ~0x80);   //Return all bits except the busy flag
}

void lcd_Data (unsigned char val){
	while(lcd_Busy());
	PTH = val;  //Put the data on port H
	LCD_WRITE   //Set up pins for writing to LCD
	LCD_DATA    //Set RS for data byte
	LCD_LATCH   //Latch the data into the LCD controller
}

void lcd_Addr (unsigned char addr){
	lcd_Ins(0x80|addr); //Set the DDRAM address
}

void lcd_AddrXY (unsigned char ix, unsigned char iy){
	switch (iy)
	{
	case 0:
		lcd_Addr( LCD_ROW0 + ix % LCD_WIDTH);
		break;
	case 1:
		lcd_Addr( LCD_ROW1 + ix % LCD_WIDTH);
		break;
	case 2:
		lcd_Addr( LCD_ROW2 + ix % LCD_WIDTH);
		break;
	case 3:
		lcd_Addr( LCD_ROW3 + ix % LCD_WIDTH);
		break;
	}
}

void lcd_String (char const * straddr){
	int index = 0;
	while (straddr[index]){
		lcd_Data(straddr[index++]);
	}
}

void lcd_StringXY (unsigned char ix, unsigned char iy, char const * const straddr){
	lcd_AddrXY(ix, iy);
	lcd_String(straddr);
}