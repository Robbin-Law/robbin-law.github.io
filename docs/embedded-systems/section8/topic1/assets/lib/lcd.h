// liquid Crystal Display
// Robbin Law
// 2025-04-05

// Small delay required on read. 
// LCD timing seems to vary on different devices
// If issues are discovered, increasing __x 
// below should be tested first.
// 20MHz Version
#define lcd_MicroDelay { char __x = 10; while (--__x); }
// Enable pin must be held high for 230ns (Datasheet Pg 52 & 58)
#define LCD_DELAY {char __x = 10; while (--__x);};

// Notice the use of bitfields instead of bit 
// masks when changing the Port K values.
// Set the LCD enable pin low
#define LCD_E_DOWN PORTK_PK0 = 0;
//Set the LCD enable pin high
#define LCD_E_UP PORTK_PK0 = 1;
// To write data R/W must be low, port H must be output
#define LCD_WRITE PORTK_PK1 = 0;DDRH = 0xFF;
// To read data R/W must be high, port H hust be input
#define LCD_READ PORTK_PK1 = 1;DDRH = 0x00; 
// RS low signals an instruction byte is being sent
#define LCD_INST PORTK_PK2 = 0;
// RS high means data byte is being sent
#define LCD_DATA PORTK_PK2 = 1;
// To latch data into the LCD, set the enable pin high,
// wait a few hundred nanoseconds, then set enable pin low
#define LCD_LATCH {LCD_E_UP LCD_DELAY LCD_E_DOWN};

////////////////////////////////////////////////////
// 8-Bit interface init on LCD
// LCD is wired to PTH for data, PK0:2 for control :
// 2     1     0     PTK 
// A     R/W*  E     LCD 
////////////////////////////////////////////////////

////////////////////////////////////////////////////
// LCD Address Scheme (HEX):
// 00 01 ... 12 13
// 40 41 ... 52 53
// 14 15 ... 26 27
// 54 55 ... 66 67
////////////////////////////////////////////////////
#define LCD_WIDTH 20
#define LCD_ROW0 0
#define LCD_ROW1 64
#define LCD_ROW2 20
#define LCD_ROW3 84

#define null 0

#define SHIFT_CUR 0
#define SHIFT_SCR (1<<3)

void lcd_Init (void);
char lcd_Busy (void);
void lcd_Ins (unsigned char);
void lcd_Clear (void);
void lcd_Home (void);
void lcd_DispControl (unsigned char cursoron, unsigned char blinkon);
char lcd_GetAddr(void);
void lcd_Data (unsigned char val);
void lcd_Addr (unsigned char addr);
void lcd_AddrXY (unsigned char ix, unsigned char iy);
void lcd_String (char const * straddr);
void lcd_StringXY (unsigned char ix, unsigned char iy, char const * const straddr);

void lcd_ShiftL (char);
void lcd_ShiftR (char);
void lcd_CGAddr (unsigned char addr);
void lcd_CGChar (unsigned char cgAddr, unsigned const char* cgData, int size);