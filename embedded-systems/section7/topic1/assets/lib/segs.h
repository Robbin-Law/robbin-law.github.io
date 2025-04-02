// Seven Segment Display
// Robbin Law
// 2025-03-30

// By default the DP (Decimal Point) for each segment is OFF
// because the MSB is 1. If it is 0 then DP is ON for that segment.
#define SEG_A 0b11000000
#define SEG_B 0b10100000 
#define SEG_C 0b10010000 
#define SEG_E 0b10001000
#define SEG_G 0b10000100  
#define SEG_F 0b10000010 
#define SEG_D 0b10000001 
// If you NEGATE this mask and then bitwise 
// AND the DP will be ON.
#define SEG_DP 0b10000000

#define DEGREE_SIGN SEG_A | SEG_B | SEG_G | SEG_F

/////////////////////////////////////////////////////////////////////////////
// Optional Custom Unions/bitfields typedef
/////////////////////////////////////////////////////////////////////////////
typedef union SevSegTypedef__
{
  unsigned char Byte;
  struct SevenSeg_Bits
  {
    unsigned int ADDR:3; 
    unsigned int BANK:1;
    unsigned int SDWN:1;
    unsigned int DEC:1;
    unsigned int HEX_B:1;
    unsigned int S_M:1;
  }Bits;
}SevSegTypedef;

typedef union Seg16Typedef__
{
  unsigned int Word;

  struct Seg16_Byte
  {//This is because our micro is Little Endian (LSB first)
    unsigned int Byte1:8;
    unsigned int Byte0:8;  
  }Byte;

  struct Seg16_Nibble
  {//This is because our micro is Little Endian (LSB first)
    unsigned int Nibble2:4;
    unsigned int Nibble3:4;
    unsigned int Nibble0:4;
    unsigned int Nibble1:4;
  }Nibble;
  
}Seg16Typedef;


/////////////////////////////////////////////////////////////////////////////
// Custom Type Definition
/////////////////////////////////////////////////////////////////////////////

// to change the mode of the controller to HEX or CUSTOM
typedef enum Segs_ModeOption_
{
  Segs_Mode_Hex = 0, 
  Segs_Mode_Custom = 1
} Segs_ModeOption;

// for functions that want to add/kill the decimal point
typedef enum Segs_DPOption_
{
  Segs_DP_OFF = 0, 
  Segs_DP_ON = 1
} Segs_DPOption;

// for functions that specify the upper or lower full display
typedef enum Segs_LineOption_
{
  Segs_LineTop = 0, 
  Segs_LineBottom = 1
} Segs_LineOption;


/////////////////////////////////////////////////////////////////////////////
// Library Prototypes
/////////////////////////////////////////////////////////////////////////////
// setup ports to speak with 7-segs
void Segs_Init (void);

// show HEX decoding or CUSTOM decoding (w/wo dp)
/* Params: (address, data, mode, dp) */
void Segs_Display (unsigned char, unsigned char, Segs_ModeOption, Segs_DPOption);

// clear the display
/* Params: (address) */
void Segs_ClearDigit(unsigned char);

// clear the entire display
void Segs_Clear(void);

// clear display  upper or lower line
/* Params: (line) */
void Segs_ClearLine(Segs_LineOption);

// show a 16-bit value on the upper or lower display
/* Params: (value, line) */
void Segs_16H (unsigned int, Segs_LineOption);

// show a decimal value on the first or second line of the 7-segs
/* Params: (value, line) */
void Segs_16D (unsigned int, Segs_LineOption);

// show the 8-bit value starting on the digit as addr (0-6)
/* Params: (addr, value) */
void Segs_8H (unsigned char, unsigned char);

// say Err on the appropriate line
/* Params: (line) */
void Segs_SayErr (Segs_LineOption line);



