// Copyleft Nabe_RMC, Meow

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>

static const uint8_t COL_MAX = 8;
static const uint8_t ROW_MAX = 14;
static const uint16_t VRAM_SIZE = 384;

extern uint8_t vram_data[VRAM_SIZE];
extern uint8_t vram_line[6 * COL_MAX];
extern uint8_t LineColor[14];
extern uint16_t UH_AddrDataTop;
extern uint8_t UB_ModeCRT;
extern uint8_t f_CRT;
extern uint8_t f_req_data_trans;
extern uint8_t f_TrgChangeColor1;
extern uint8_t f_TrgChangeColor2;
extern uint8_t f_TrgChangeColor3;
extern uint8_t f_disp_on;
extern uint8_t UB_Color;
extern volatile uint8_t UB_Color_DRAW0;
extern volatile uint8_t UB_Color_DRAW1;
extern volatile uint8_t UB_Color_DRAW2;
extern volatile uint8_t UB_Color_DRAW3;

class CINT
{
public:
    CINT();

    void Ini();
    void JobAsy();
    void ExecFunc(uint8_t);
    uint8_t Check66ms();
    uint8_t Check1056ms();

    void Int_ICF1_RGB_TEST();
    void Int_ICF1_RGB_NUCO();
    void Int_ICF1_RGB_TEXT1();
    void Int_ICF1_RGB_TEXT2();
    void Int_ICF1_RGB_DRAW();
    void Int_ICF1_Reserve();
    void FitDelay(uint8_t);

    void OutData_64(uint16_t);
    void OutData_70(uint16_t);
    void OutTxt_JC(uint16_t, uint8_t, uint8_t);
    void OutTxtRGB_1(uint16_t, uint8_t, uint8_t);
    void OutTxtRGB_2(uint16_t, uint8_t, uint8_t);
    void OutTxtCRT_1(uint16_t, uint8_t, uint8_t);
    void OutDelay(uint16_t);
    void OutDraw(uint8_t, uint8_t, uint8_t);

    uint16_t UH_AddrDataNow;

    PROGMEM static const uint8_t cg_code[ (0x7F - 0x00 +1)*6 ];
    PROGMEM static const uint8_t MASK_bit[ 11 ];
    PROGMEM static const uint8_t CHAR_ROW[10*10];
    PROGMEM static const uint8_t CG_LINE[10*10];
    PROGMEM static const uint8_t VGA_LINE_TBL[520];


private:
    uint16_t  UH_TCNT1;
    uint16_t  UH_hline;
    uint8_t UB_scanline;
    uint8_t UB_char_row;
    uint8_t UB_cg_line;
    static volatile uint8_t f_SYS_66ms_req;
    static volatile uint8_t f_SYS_1056ms_req;

};

#define P_VSYNC_ON      asm volatile("sbi 0x05,0"::)
#define P_VSYNC_OFF     asm volatile("cbi 0x05,0"::)


static const uint8_t OR_MCUCR = 0x10;
static const uint8_t OR_DDRB = 0x2F;
static const uint8_t AND_DDRB = 0xEF;
static const uint8_t AND_PORTB = 0xFD;
static const uint8_t AND_PORTC = 0x00;
static const uint8_t OR_DDRC_COL_6B = 0x3F;
static const uint8_t AND_PORTD = 0x00;
static const uint8_t OR_DDRD_GSC_4B = 0xFE;
static const uint8_t AND_DDRD_GSC_4B = 0xFE;
static const uint8_t CRT_TCCR1A = 0xC2;
static const uint8_t CRT_TCCR1B = 0x19;
static const uint8_t CRT_OCR1A_HS = 61;
static const uint16_t CRT_ICR1 = 508;

static const uint8_t CRT_TEST1 = 0;
static const uint8_t CRT_TEST2 = 1;
static const uint8_t CRT_NUCO1 = 2;
static const uint8_t CRT_NUCO2 = 3;
static const uint8_t CRT_TEXT = 4;
static const uint8_t CRT_DRAW = 5;
static const uint8_t CRT_RSV = 6;
static const uint8_t CRT_N = 7;
static const uint8_t CRT_UP = 20;



static const uint8_t COL_RED = 0x03;
static const uint8_t COL_GRN = 0x0C;
static const uint8_t COL_BLU = 0x30;
static const uint8_t COL_YEL = 0x0F;
static const uint8_t COL_CYA = 0x3C;
static const uint8_t COL_MAG = 0x33;
static const uint8_t COL_WHI = 0x3F;

static const uint8_t V_SYNC_END = 0xA9;
static const uint8_t V_LINE_END = 0xAA;
static const uint8_t CG_NONE = 0xAF;

static const uint8_t CG_LINE_0 = 0xA0;
static const uint8_t CG_LINE_1 = 0xA1;
static const uint8_t CG_LINE_2 = 0xA2;
static const uint8_t CG_LINE_00 = 0xA3;

static const uint8_t H_DOT_N_VGA_A = 70;
static const uint8_t H_NUM_FRM_VGA_A = 64;
static const uint8_t H_NUM_TXT_ROW = 16;
static const uint8_t V_SYNC_START_VGA_A = 0;
static const uint8_t V_SYNC_END_VGA_A = 2;
static const uint16_t V_BLANK_VGA_A = V_SYNC_END_VGA_A + 68;
static const uint16_t H_LINE_FRM_VGA_A = V_BLANK_VGA_A + H_NUM_FRM_VGA_A * 6;
static const uint16_t H_LINE_FRM_VGA_NUCO = V_BLANK_VGA_A + H_NUM_FRM_VGA_A * 6;
static const uint16_t H_LINE_TXT_END = V_BLANK_VGA_A + H_NUM_TXT_ROW *20;
static const uint16_t H_LINE_END_VGA_A = 520;

static const uint8_t V_SYNC_END_DRAW = 2;
static const uint8_t V_BLANK_DRAW = 70;
static const uint16_t H_LINE_FRM_DRAW = 454;
static const uint16_t H_LINE_END_DRAW = 520;

uint8_t UB_ModeColor;

class CCRT
{
public:
    CCRT();
    void Ini();
    void ChangeMode(uint8_t);
    void Set_Data_sin();
    void Set_Data_1(uint16_t, uint8_t);
    PROGMEM static const uint8_t TEST1_data[ 64*3 ];
    PROGMEM static const uint8_t AyaNuco1[ 70 * 64 ];
    PROGMEM static const uint8_t AyaNuco2[ 70 * 64 ];
    PROGMEM static const uint8_t VGA_SIN_TBL[256];
};


class CCSL
{
public:
    CCSL();
    void Ini();
    void ClearTxt();
    void cursor(int, int);
    uint8_t put_char(uint8_t, uint8_t, uint8_t);
    uint8_t set_line_color(uint8_t, uint8_t);
private:
    int numRows;
    int numCols;
};

class CSYS
{
public:
    CSYS();
    uint8_t f_CycleEnd;
    uint16_t UH_PagePic;
    uint8_t UB_ReadByte;
    uint8_t UB_ReadByte_1;
    uint8_t UB_ReadByte_2;
    uint8_t UB_EepData0;
    uint8_t UB_EepData1;
    void Ini();
    void delay66(uint8_t);
private:
    static void IniPort();
    static void IniTimer();
    static void IniINT();
};

CSYS objSYS;
CCRT objCRT;
CCSL objCSL;
CINT objINT;

void SetRndPosition( void );
void SetColor(void);

long randNumber;
unsigned char x_typA, y_typA;
unsigned char x_typB, y_typB;
unsigned char x_typC, y_typC;
unsigned char x_typH, y_typH;

void loop();

uint8_t random(uint8_t x, uint8_t y)
{
    uint8_t ret = (rand() % (y - x)) + x;
    return ret;
}

int main()
{
    objSYS.Ini();               /* re-initialize after Arduini IDE init()   */
    objCRT.ChangeMode( CRT_TEXT );
    srand(1);
	//randomSeed(analogRead(0));
    sei();                      /* interrupt enable                         */
    
    while (true)
        loop();
}

void loop()
{
	int i,j,k;
	uint8_t LineColor;

	SetColor();
    objCSL.put_char( 0, 0, 'A' );	objSYS.delay66( 5 );	/* line 1	*/
    objCSL.put_char( 1, 0, 'r' );	objSYS.delay66( 5 );
    objCSL.put_char( 2, 0, 'd' );	objSYS.delay66( 5 );
    objCSL.put_char( 3, 0, 'u' );	objSYS.delay66( 5 );
    objCSL.put_char( 4, 0, 'i' );	objSYS.delay66( 5 );
    objCSL.put_char( 5, 0, 'n' );	objSYS.delay66( 5 );
    objCSL.put_char( 6, 0, 'o' );	objSYS.delay66( 20 );
	
	objCSL.put_char( 0, 1, 'V' );	objSYS.delay66( 2 );	/* line 2   */
    objCSL.put_char( 1, 1, 'G' );	objSYS.delay66( 3 );
    objCSL.put_char( 2, 1, 'A' );	objSYS.delay66( 4 );
	objCSL.put_char( 3, 1, '_' );	objSYS.delay66( 5 );
    objCSL.put_char( 4, 1, 'o' );	objSYS.delay66( 6 );
    objCSL.put_char( 5, 1, 'u' );	objSYS.delay66( 7 );
    objCSL.put_char( 6, 1, 't' );	objSYS.delay66( 8 );
	
    objCSL.put_char( 0, 2, '2' );	objSYS.delay66( 5 );      /* line 3   */
    objCSL.put_char( 1, 2, '0' );	objSYS.delay66( 5 );
    objCSL.put_char( 2, 2, '1' );	objSYS.delay66( 5 );
    objCSL.put_char( 3, 2, '3' );	objSYS.delay66( 10 );
	
	objCSL.put_char( 0, 3, 'N' );      /* line 4   */
    objCSL.put_char( 1, 3, 'a' );
    objCSL.put_char( 2, 3, 'b' );
	objCSL.put_char( 3, 3, 'e' );
    objCSL.put_char( 4, 3, '_' );
    objCSL.put_char( 5, 3, 'R' );
    objCSL.put_char( 6, 3, 'M' );
    objCSL.put_char( 7, 3, 'C' );	objSYS.delay66( 10 );
	
   	objCSL.put_char( 0, 13, 0x0E );
  	objCSL.put_char( 1, 13, 0x0F );objSYS.delay66( 5 );
   	objCSL.put_char( 3, 13, 0x10 );
  	objCSL.put_char( 4, 13, 0x11 );objSYS.delay66( 5 );	
   	objCSL.put_char( 6, 13, 0x0E );
  	objCSL.put_char( 7, 13, 0x0F );objSYS.delay66( 5 );
	
    while(1)
    {
        for( i = 0; i < 10; i++ )
        {
            SetRndPosition();
			
			for (k = 0; k<14; k++)
                objCSL.set_line_color(k, (uint8_t)random(0, 63));
			
			for (i = 4*COL_MAX; i < (13*COL_MAX-1); i++)
                vram_data[i] = 0x20;
			
            for (j = 0; j <4; j++)
            {
                objCSL.put_char( x_typH, y_typH, 0x12 );
                objCSL.put_char( x_typH+1, y_typH, 0x13 );
		   		objCSL.put_char( x_typA, y_typA, 0x00 );
		   		objCSL.put_char( x_typA+1, y_typA, 0x01 );
		   		objCSL.put_char( x_typB, y_typB, 0x04 );
		   		objCSL.put_char( x_typB+1, y_typB, 0x05 );
 		  		objCSL.put_char( x_typC, y_typC, 0x08 );
 				objCSL.put_char( x_typC+1, y_typC, 0x09 );
				objSYS.delay66( 2 );
				
		   		objCSL.put_char( x_typH, y_typH, 0x12 );
		   		objCSL.put_char( x_typH+1, y_typH, 0x13 );
		   		objCSL.put_char( x_typA, y_typA, 0x02 );
		   		objCSL.put_char( x_typA+1, y_typA, 0x03 );
		   		objCSL.put_char( x_typB, y_typB, 0x06 );
		   		objCSL.put_char( x_typB+1, y_typB, 0x07 );
 		  		objCSL.put_char( x_typC, y_typC, 0x0A );
 				objCSL.put_char( x_typC+1, y_typC, 0x0B );	
				objSYS.delay66( 5 );
			}
		}
	}

}

void CCRT::ChangeMode(uint8_t mode)
{
    static uint8_t tmp = 0;

    switch (mode)
    {
        case CRT_TEST1:
            UB_ModeCRT = CRT_TEST1;
            UH_AddrDataTop = (uint16_t)(&TEST1_data);
            break;
        case CRT_UP:
            UB_ModeCRT = CRT_TEST2;
            tmp = (tmp+1) & 0x3F;
            UH_AddrDataTop = ((uint16_t)(&TEST1_data) +tmp);
            break;
         case CRT_NUCO1:
            UB_ModeCRT = CRT_NUCO1;
            UH_AddrDataTop = (uint16_t)(&AyaNuco1);
            break;
         case CRT_NUCO2:
            UB_ModeCRT = CRT_NUCO1;
            UH_AddrDataTop = (uint16_t)(&AyaNuco2);
            break;
         case CRT_TEXT:
            UB_ModeCRT = CRT_TEXT;
            UH_AddrDataTop = (uint16_t)(&vram_line);
            break;
         case CRT_DRAW:
            UB_ModeCRT = CRT_DRAW;
            break;
        default:
            UB_ModeCRT = CRT_TEST1;
            UH_AddrDataTop = (uint16_t)(&TEST1_data);
    }
}

void SetColor(void)
{
    objCSL.set_line_color(0, COL_RED);
    objCSL.set_line_color(1, COL_GRN);
    objCSL.set_line_color(2, COL_BLU);
    objCSL.set_line_color(3, COL_YEL);
    objCSL.set_line_color(4, COL_MAG);
    objCSL.set_line_color(5, COL_CYA);
    objCSL.set_line_color(6, COL_WHI);
    objCSL.set_line_color(7, 0x2B);     /*  0010_1011B  */
    objCSL.set_line_color(8, 0x2E);     /*  0010_1110B  */
    objCSL.set_line_color(9, 0x3A);     /*  0011_1010B  */
    objCSL.set_line_color(10, 0x2F);    /*  0010_1111B  */
    objCSL.set_line_color(11, 0x3E);    /*  0011_1110B  */
    objCSL.set_line_color(12, 0x3B);    /*  0011_1011B  */
    objCSL.set_line_color(13, 0x2E);    /*  0010_1111B  */
}

void SetRndPosition(void)
{
#if 0
    x_typH = y_typH = x_typA = y_typA = x_typB = y_typB = x_typC = y_typC = 4;
#else
	x_typH = (uint8_t)random( 0, 6 );
	y_typH = (uint8_t)random( 4, 12 );
	x_typA = (uint8_t)random( 0, 6 );
	y_typA = (uint8_t)random( 4, 12 );
	x_typB = (uint8_t)random( 0, 6 );
	y_typB = (uint8_t)random( 4, 12 );
	x_typC = (uint8_t)random( 0, 6 );
	y_typC = (uint8_t)random( 4, 12 );
#endif
}

CCSL::CCSL()
{
    Ini();
}

void CCSL::Ini()
{
    ClearTxt();
}

void CCSL::ClearTxt()
{
    for (int i = 0; i < COL_MAX*ROW_MAX; i++)
        vram_data[i] = 0x20;
}

void CCSL::cursor(int line, int col){

    /* comoing soon */
}


uint8_t CCSL::put_char(uint8_t x, uint8_t y, uint8_t c)
{
    if ((c > 0x7F)||(x >7)||(y > 13))
        return -1;

    vram_data[8*y+x] = c;
    return 0 ;
}

uint8_t CCSL::set_line_color(uint8_t line, uint8_t color)
{
    if ((line >13) || (color > 63))
        return -1;

    LineColor[line] = color;
    return 0;
}

uint8_t vram_data[VRAM_SIZE];
uint8_t vram_line[ 6 * COL_MAX ];
uint8_t LineColor[ 14 ];

uint16_t UH_AddrDataTop;
uint8_t UB_ModeCRT;
uint8_t f_CRT;
uint8_t f_req_data_trans;
uint8_t f_TrgChangeColor1;
uint8_t f_TrgChangeColor2;
uint8_t f_TrgChangeColor3;
uint8_t f_disp_on;
uint8_t UB_Color;
volatile uint8_t UB_Color_DRAW0;
volatile uint8_t UB_Color_DRAW1;
volatile uint8_t UB_Color_DRAW2;
volatile uint8_t UB_Color_DRAW3;

CINT SobjINT;

CSYS::CSYS()
{
    f_CycleEnd = 1;
}

void CSYS::Ini()
{
    IniPort();
    IniTimer();
    IniINT();
}

void CSYS::IniPort()
{
    MCUCR |= OR_MCUCR;
    PORTB &= AND_PORTB;
    DDRB &= AND_DDRB;
    DDRB |= OR_DDRB;
    PORTC &= AND_PORTC;
    DDRC |= OR_DDRC_COL_6B;
    PORTD &= AND_PORTD;
    DDRD &= AND_DDRD_GSC_4B;
    DDRD |= OR_DDRD_GSC_4B;
}

void CSYS::IniTimer()
{
    TCCR1A = CRT_TCCR1A;
    TCCR1B = CRT_TCCR1B;
    OCR1A = CRT_OCR1A_HS;
    ICR1 = CRT_ICR1;
}

void CSYS::IniINT()
{
    TIMSK1 = 0x20;
    TIMSK0 = 0;
}

void CSYS::delay66(uint8_t count)
{
    while (count != 0)
    {
        while (SobjINT.Check66ms() == 0)
            ;

        count--;
    }
}

PROGMEM const uint8_t CINT::cg_code[ (0x7F - 0x00 +1)*6 ] = {
0x98,0x5C,0xB6,0x5F,0x5F,0xB6,  /* 00H type A1_L */
0x5C,0x98,0x00,0x00,0x00,0x00,  /* 01H type A1_R */
0x58,0xBC,0x16,0x3F,0x3F,0x16,  /* 02H type A2_L */
0xBC,0x58,0x00,0x00,0x00,0x00,  /* 03H type A2_R */
0x1E,0x98,0x7D,0x36,0x3C,0x3C,  /* 04H type B1_L */
0x3C,0x36,0x7D,0x98,0x1E,0x00,  /* 05H type B1_R */
0x70,0x18,0x7D,0xB6,0xBC,0x3C,  /* 06H type B2_L */
0xBC,0xB6,0x7D,0x18,0x70,0x00,  /* 07H type B2_R */
0x9C,0x9E,0x5E,0x76,0x37,0x5F,  /* 08H type C1_L */
0x5F,0x37,0x76,0x5E,0x9E,0x9C,  /* 09H type C1_R */
0x1C,0x5E,0xFE,0xB6,0x37,0x5F,  /* 0AH type C2_L */
0x5F,0x37,0xB6,0xFE,0x5E,0x1C,  /* 0BH type C2_R */
0x04,0x49,0x22,0x14,0x41,0x22,  /* 0CH crash_L   */
0x22,0x41,0x14,0x22,0x49,0x04,  /* 0DH crash_R   */
0xF8,0xFC,0xFE,0x7F,0x3F,0x3F,  /* 0EH silo_L    */
0x3F,0x3F,0x7F,0xFE,0xFC,0xF8,  /* 0FH silo_R    */
0xE0,0xF0,0xF0,0xF8,0xF8,0xFC,  /* 10H turret_L  */
0xF8,0xF8,0xF0,0xF0,0xE0,0x00,  /* 11H turret_R  */
0x00,0x0E,0x1F,0x3F,0x3F,0x7E,  /* 12H heart_L   */
0xFC,0x7E,0x3F,0x3F,0x1E,0x0E,  /* 13H heart_R   */
0x00,0x00,0x00,0x00,0x00,0x00,  /* 14h ' '       */
0x00,0x00,0x00,0x00,0x00,0x00,  /* 15h ' '       */
0x00,0x00,0x00,0x00,0x00,0x00,  /* 16h ' '       */
0x00,0x32,0x49,0x79,0x41,0x3E,  /* 17H '@', */
0x00,0x7F,0x04,0x08,0x10,0x7F,  /* 18H 'N', */
0x00,0x10,0x2A,0x2A,0x2A,0x3C,  /* 19H 'a', */
0x00,0x3F,0x24,0x24,0x24,0x18,  /* 1AH 'b', */
0x00,0x1C,0x2A,0x2A,0x2A,0x2C,  /* 1BH 'e', */
0x40,0x40,0x40,0x40,0x40,0x40,  /* 1CH '_', */
0x00,0x7F,0x09,0x19,0x29,0x46,  /* 1DH 'R', */
0x00,0x7F,0x02,0x0C,0x02,0x7F,  /* 1EH 'M', */
0x00,0x3E,0x41,0x41,0x41,0x22,  /* 1FH 'C', */
0x00,0x00,0x00,0x00,0x00,0x00,  /* 20H ' ', */
0x00,0x00,0x00,0x4F,0x00,0x00,  /* 21H '!', */
0x00,0x00,0x07,0x00,0x07,0x00,  /* 22H '"', */
0x00,0x14,0x7F,0x14,0x7F,0x14,  /* 23H '#', */
0x00,0x24,0x2A,0x7F,0x2A,0x12,  /* 24H '$', */
0x00,0x23,0x13,0x08,0x64,0x62,  /* 25H '%', */
0x00,0x36,0x49,0x55,0x22,0x50,  /* 26H '&', */
0x00,0x00,0x00,0x07,0x00,0x00,  /* 27H ''', */
0x00,0x00,0x1C,0x22,0x41,0x00,  /* 28H '(', */
0x00,0x00,0x41,0x22,0x1C,0x00,  /* 29H ')', */
0x00,0x54,0x38,0x10,0x38,0x54,  /* 2AH '*', */
0x00,0x10,0x10,0x7C,0x10,0x10,  /* 2BH '+', */
0x00,0x40,0x30,0x00,0x00,0x00,  /* 2CH ',', */
0x08,0x08,0x08,0x08,0x08,0x08,  /* 2DH '-', */
0x00,0x00,0x00,0x30,0x00,0x00,  /* 2EH '.', */
0x00,0x20,0x10,0x08,0x04,0x02,  /* 2FH '/', */
0x00,0x3E,0x51,0x49,0x45,0x3E,  /* 30H '0', */
0x00,0x00,0x82,0xFF,0x80,0x00,  /* 31H '1', */
0x00,0x42,0x61,0x51,0x49,0x46,  /* 32H '2', */
0x00,0x21,0x41,0x45,0x4B,0x31,  /* 33H '3', */
0x00,0x18,0x14,0x12,0x7F,0x10,  /* 34H '4', */
0x00,0x67,0x45,0x45,0x45,0x39,  /* 35H '5', */
0x00,0x3C,0x4A,0x49,0x49,0x30,  /* 36H '6', */
0x00,0x01,0x01,0x71,0x09,0x07,  /* 37H '7', */
0x00,0x36,0x49,0x49,0x49,0x36,  /* 38H '8', */
0x00,0x06,0x49,0x49,0x69,0x1E,  /* 39H '9', */
0x00,0x00,0x00,0x36,0x00,0x00,  /* 3AH ':', */
0x00,0x00,0x40,0x36,0x00,0x00,  /* 3BH ';', */
0x00,0x08,0x14,0x22,0x41,0x00,  /* 3CH '<', */
0x00,0x14,0x14,0x14,0x14,0x14,  /* 3DH '=', */
0x00,0x00,0x41,0x22,0x14,0x08,  /* 3EH '>', */
0x00,0x02,0x01,0x51,0x09,0x06,  /* 3FH '?', */
0x00,0x32,0x49,0x79,0x41,0x3E,  /* 40H '@', */
0x00,0x7C,0x12,0x11,0x12,0x7C,  /* 41H 'A', */
0x00,0x7F,0x49,0x49,0x49,0x36,  /* 42H 'B', */
0x00,0x3E,0x41,0x41,0x41,0x22,  /* 43H 'C', */
0x00,0x7F,0x41,0x41,0x22,0x1C,  /* 44H 'D', */
0x00,0x7F,0x49,0x49,0x49,0x41,  /* 45H 'E', */
0x00,0x7F,0x09,0x09,0x09,0x01,  /* 46H 'F', */
0x00,0x3E,0x41,0x49,0x49,0x3A,  /* 47H 'G', */
0x00,0x7F,0x08,0x08,0x08,0x7F,  /* 48H 'H', */
0x00,0x00,0x41,0x7F,0x41,0x00,  /* 49H 'I', */
0x00,0x20,0x40,0x41,0x3F,0x01,  /* 4AH 'J', */
0x00,0x7F,0x08,0x14,0x22,0x41,  /* 4BH 'K', */
0x00,0x7F,0x40,0x40,0x40,0x40,  /* 4CH 'L', */
0x00,0x7F,0x02,0x0C,0x02,0x7F,  /* 4DH 'M', */
0x00,0x7F,0x04,0x08,0x10,0x7F,  /* 4EH 'N', */
0x00,0x3E,0x41,0x41,0x41,0x3E,  /* 4FH 'O', */
0x00,0x7F,0x09,0x09,0x09,0x06,  /* 50H 'P', */
0x00,0x7F,0x41,0x51,0x21,0x5E,  /* 51H 'Q', */
0x00,0x7F,0x09,0x19,0x29,0x46,  /* 52H 'R', */
0x00,0x46,0x49,0x49,0x49,0x71,  /* 53H 'S', */
0x00,0x01,0x01,0x7F,0x01,0x01,  /* 54H 'T', */
0x00,0x1F,0x20,0x40,0x20,0x1F,  /* 55H 'U', */
0x00,0x1F,0x20,0x40,0x20,0x1F,  /* 56H 'V', */
0x00,0x3F,0x40,0x38,0x40,0x3F,  /* 57H 'W', */
0x00,0x63,0x14,0x08,0x14,0x63,  /* 58H 'X', */
0x00,0x07,0x08,0x70,0x08,0x07,  /* 59H 'Y', */
0x00,0x61,0x51,0x49,0x45,0x43,  /* 5AH 'Z', */
0x00,0x7F,0x41,0x41,0x41,0x00,  /* 5BH '[', */
0x00,0x02,0x04,0x08,0x10,0x20,  /* 5CH '\', */
0x00,0x41,0x41,0x41,0x7F,0x00,  /* 5DH ']', */
0x00,0x04,0x02,0x01,0x02,0x04,  /* 5EH '^', */
0x00,0x40,0x40,0x40,0x40,0x40,  /* 5FH '_', */
0x00,0x00,0x01,0x02,0x04,0x00,  /* 60H '`', */
0x00,0x10,0x2A,0x2A,0x2A,0x3C,  /* 61H 'a', */
0x00,0x3F,0x24,0x24,0x24,0x18,  /* 62H 'b', */
0x00,0x1C,0x22,0x22,0x22,0x22,  /* 63H 'c', */
0x00,0x18,0x24,0x24,0x24,0x3F,  /* 64H 'd', */
0x00,0x1C,0x2A,0x2A,0x2A,0x2C,  /* 65H 'e', */
0x00,0x08,0x3E,0x09,0x09,0x02,  /* 66H 'f', */
0x00,0x0C,0x52,0x52,0x52,0x3E,  /* 67H 'g', */
0x00,0x3F,0x04,0x04,0x04,0x38,  /* 68H 'h', */
0x00,0x00,0x24,0x3D,0x20,0x00,  /* 69H 'i', */
0x00,0x10,0x20,0x22,0x1F,0x00,  /* 6AH 'j', */
0x00,0x3F,0x08,0x08,0x14,0x22,  /* 6BH 'k', */
0x00,0x00,0x21,0x3F,0x20,0x00,  /* 6CH 'l', */
0x00,0x3C,0x02,0x3E,0x02,0x3C,  /* 6DH 'm', */
0x00,0x3E,0x04,0x02,0x02,0x3C,  /* 6EH 'n', */
0x00,0x38,0x44,0x44,0x44,0x38,  /* 6FH 'o', */
0x00,0x7E,0x12,0x12,0x12,0x0C,  /* 70H 'p', */
0x00,0x0C,0x12,0x12,0x12,0x7E,  /* 71H 'q', */
0x00,0x3E,0x04,0x02,0x02,0x04,  /* 72H 'r', */
0x00,0x24,0x2A,0x2A,0x2A,0x12,  /* 73H 's', */
0x00,0x04,0x1F,0x24,0x24,0x14,  /* 74H 't', */
0x00,0x1E,0x20,0x20,0x20,0x3E,  /* 75H 'u', */
0x00,0x0E,0x10,0x20,0x10,0x0E,  /* 76H 'v', */
0x00,0x1E,0x20,0x18,0x20,0x1E,  /* 77H 'w', */
0x00,0x22,0x14,0x08,0x14,0x22,  /* 78H 'x', */
0x00,0x06,0x48,0x30,0x08,0x06,  /* 79H 'y', */
0x00,0x22,0x32,0x2A,0x26,0x22,  /* 7AH 'z', */
0x00,0x08,0x36,0x41,0x41,0x00,  /* 7BH '{', */
0x00,0x00,0x00,0x77,0x00,0x00,  /* 7CH '|', */
0x00,0x00,0x41,0x41,0x36,0x08,  /* 7DH '}', */
0x00,0x02,0x01,0x03,0x02,0x01,  /* 7EH '~', */
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,  /* 7FH '^?', */
};

PROGMEM const uint8_t CINT::VGA_LINE_TBL[ 520 ] = {
0xA8,0xAF,0xA9,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,
0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,
0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,
0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,
0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xA3,0xA0,0xA0,0xA1,0xA1,0xA1,0xA2,0xA2,0xA2,0x01,
0x01,0x01,0x02,0x02,0x02,0x04,0x04,0x04,0x08,0x08,0x08,0x10,0x10,0x10,0x20,0x20,
0x20,0x40,0x40,0x40,0xA3,0xA0,0xA0,0xA1,0xA1,0xA1,0xA2,0xA2,0xA2,0x01,0x01,0x01,
0x02,0x02,0x02,0x04,0x04,0x04,0x08,0x08,0x08,0x10,0x10,0x10,0x20,0x20,0x20,0x40,
0x40,0x40,0xA3,0xA0,0xA0,0xA1,0xA1,0xA1,0xA2,0xA2,0xA2,0x01,0x01,0x01,0x02,0x02,
0x02,0x04,0x04,0x04,0x08,0x08,0x08,0x10,0x10,0x10,0x20,0x20,0x20,0x40,0x40,0x40,
0xA3,0xA0,0xA0,0xA1,0xA1,0xA1,0xA2,0xA2,0xA2,0x01,0x01,0x01,0x02,0x02,0x02,0x04,
0x04,0x04,0x08,0x08,0x08,0x10,0x10,0x10,0x20,0x20,0x20,0x40,0x40,0x40,0xA3,0xA0,
0xA0,0xA1,0xA1,0xA1,0xA2,0xA2,0xA2,0x01,0x01,0x01,0x02,0x02,0x02,0x04,0x04,0x04,
0x08,0x08,0x08,0x10,0x10,0x10,0x20,0x20,0x20,0x40,0x40,0x40,0xA3,0xA0,0xA0,0xA1,
0xA1,0xA1,0xA2,0xA2,0xA2,0x01,0x01,0x01,0x02,0x02,0x02,0x04,0x04,0x04,0x08,0x08,
0x08,0x10,0x10,0x10,0x20,0x20,0x20,0x40,0x40,0x40,0xA3,0xA0,0xA0,0xA1,0xA1,0xA1,
0xA2,0xA2,0xA2,0x01,0x01,0x01,0x02,0x02,0x02,0x04,0x04,0x04,0x08,0x08,0x08,0x10,
0x10,0x10,0x20,0x20,0x20,0x40,0x40,0x40,0xA3,0xA0,0xA0,0xA1,0xA1,0xA1,0xA2,0xA2,
0xA2,0x01,0x01,0x01,0x02,0x02,0x02,0x04,0x04,0x04,0x08,0x08,0x08,0x10,0x10,0x10,
0x20,0x20,0x20,0x40,0x40,0x40,0xA3,0xA0,0xA0,0xA1,0xA1,0xA1,0xA2,0xA2,0xA2,0x01,
0x01,0x01,0x02,0x02,0x02,0x04,0x04,0x04,0x08,0x08,0x08,0x10,0x10,0x10,0x20,0x20,
0x20,0x40,0x40,0x40,0xA3,0xA0,0xA0,0xA1,0xA1,0xA1,0xA2,0xA2,0xA2,0x01,0x01,0x01,
0x02,0x02,0x02,0x04,0x04,0x04,0x08,0x08,0x08,0x10,0x10,0x20,0x20,0x20,0x40,0x40,
0x40,0xA3,0xA0,0xA0,0xA1,0xA1,0xA1,0xA2,0xA2,0xA2,0x01,0x01,0x01,0x02,0x02,0x02,
0x04,0x04,0x04,0x08,0x08,0x08,0x10,0x10,0x10,0x20,0x20,0x20,0x40,0x40,0x40,0xA3,
0xA0,0xA0,0xA1,0xA1,0xA1,0xA2,0xA2,0xA2,0x01,0x01,0x01,0x02,0x02,0x02,0x04,0x04,
0x04,0x08,0x08,0x08,0x10,0x10,0x10,0x20,0x20,0x20,0x40,0x40,0x40,0xA3,0xA0,0xA0,
0xA1,0xA1,0xA1,0xA2,0xA2,0xA2,0x01,0x01,0x01,0x02,0x02,0x02,0x04,0x04,0x04,0x08,
0x08,0x08,0x10,0x10,0x10,0x20,0x20,0x20,0x40,0x40,0x40,0xA3,0xA0,0xA0,0xA1,0xA1,
0xA1,0xA2,0xA2,0xA2,0x01,0x01,0x01,0x02,0x02,0x02,0x04,0x04,0x04,0x08,0x08,0x08,
0x10,0x10,0x10,0x20,0x20,0x20,0x40,0x40,0x40,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,
0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,
0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAF,0xAA
};

PROGMEM const uint8_t CINT::MASK_bit[ 11 ] = {
    0, 0, 0, 0x01, 0x02, 0x04, 0x8, 0x10, 0x20, 0x40, 0x00
};

CINT::CINT()
{
    Ini();
}

volatile uint8_t CINT::f_SYS_1056ms_req = 0;
volatile uint8_t CINT::f_SYS_66ms_req = 0;

void CINT::Ini( void )
{
    UB_ModeCRT = 0;
    UB_Color = 0x3F;
    LineColor[0] = 0x03;
    LineColor[1] = 0x0C;
    LineColor[2] = 0x0F;
    LineColor[3] = 0x30;
    LineColor[4] = 0x3C;
    LineColor[5] = 0x33;
    LineColor[6] = 0x3F;
    LineColor[7] = 0x13;
    LineColor[8] = 0x1C;
    LineColor[9] = 0x31;
    LineColor[10] = 0x23;
    LineColor[11] = 0x2C;
    LineColor[12] = 0x32;
    LineColor[13] = 0x28;
}

uint8_t CINT::Check1056ms()
{
    if (f_SYS_1056ms_req == 0)
        return 0;

    f_SYS_1056ms_req = 0;
    return 1;
}

uint8_t CINT::Check66ms()
{
    if (f_SYS_66ms_req == 0)
        return 0;

    f_SYS_66ms_req =0;
    return 1;
}

void CINT::Int_ICF1_RGB_DRAW()
{   
    static uint16_t Position_x = 0;
    static uint8_t delay = 0;
    UH_hline++;

    if ((UH_hline >= V_BLANK_DRAW) && (UH_hline < H_LINE_FRM_DRAW))
    {
        PORTC = UB_Color_DRAW0; 
        delay = vram_data[Position_x]; 
        OutDraw(delay, UB_Color_DRAW1, UB_Color_DRAW2);
        PORTC = UB_Color_DRAW3;
        Position_x++;
    }
    else if (UH_hline >= H_LINE_END_DRAW)
    {
        UH_hline = 0;
        P_VSYNC_OFF;
        JobAsy();
        Position_x = 0;
    }
    else if (UH_hline == V_SYNC_END_DRAW)
    {
        P_VSYNC_ON;
    }
}

void CINT::Int_ICF1_RGB_TEXT1()
{
    static uint8_t p_vram_line = 0;
    static uint8_t p_vram_txt = 0;
    static uint16_t p_line_top;
    static uint16_t dsp_char = 0;
    static uint8_t isMabiki = 0;
    uint8_t i;
    uint8_t mask_bit_data;
    UH_hline++;

    if (UH_hline == H_LINE_END_VGA_A)
    {
        UH_hline = 0;
        UB_scanline = 0;
        p_vram_txt = 0;
        UB_Color = 0x03;
    }

    if ((UH_hline >= V_BLANK_VGA_A) && (UH_hline < H_LINE_TXT_END))
    {
        UB_char_row = UB_scanline/20;
        UB_cg_line = (UB_scanline % 20)/2;

        if (UB_cg_line == 0)
        {
                p_vram_line = 0;
                p_line_top = (uint16_t)UB_char_row * (uint16_t)COL_MAX;
                p_vram_txt = 0;
                UB_Color += 0x02A;
        }

        else if (UB_cg_line == 1)
        {
            dsp_char = 6* (vram_data[p_vram_txt]);
            vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char]);
            vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+1]);
            vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+2]);
            vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+3]);
            vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+4]);
            vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+5]);
            p_vram_txt++;
        }
        else if (UB_cg_line == 2)
        {
            dsp_char = 6* (vram_data[p_vram_txt]);
            vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char]);
            vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+1]);
            vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+2]);
            vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+3]);
            vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+4]);
            vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+5]);
            p_vram_txt++;
        }
        else
        {
            mask_bit_data = pgm_read_byte(&MASK_bit[UB_cg_line]);
            OutTxt_JC(UH_AddrDataNow, mask_bit_data, UB_Color);
        }

        PORTC = 0x00;
        UB_scanline++;
    }
    else if (UH_hline == V_SYNC_START_VGA_A)
    {
        P_VSYNC_OFF;
        JobAsy();
        UH_AddrDataNow = UH_AddrDataTop;
    }
    else if (UH_hline == V_SYNC_END_VGA_A)
    {
        P_VSYNC_ON;
    }
}

void CINT::Int_ICF1_RGB_TEXT2()
{   
    static uint8_t Mode_Line = 0;
    static uint8_t p_vram_line = 0;
    static uint8_t p_vram_txt = 0;
    static uint16_t dsp_char = 0;
    static uint16_t num_line = 0;
    Mode_Line = pgm_read_byte(&VGA_LINE_TBL[UH_hline]);
    UH_hline++;
    
    switch (Mode_Line)
    {
        case V_SYNC_END:
            P_VSYNC_ON;
            break;
        case V_LINE_END:
            P_VSYNC_OFF;
            UH_hline = 0;
            JobAsy();
            UH_AddrDataNow = UH_AddrDataTop;
            p_vram_txt = 0;
            num_line = 0;
            break;
        case CG_LINE_00:
            p_vram_line = 0;
            UB_Color = LineColor[num_line];
            num_line++;
            break;
        case CG_LINE_0:
dsp_char = 6* (vram_data[p_vram_txt]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+1]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+2]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+3]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+4]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+5]);
p_vram_txt++;
            break;
        case CG_LINE_1:
dsp_char = 6* (vram_data[p_vram_txt]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+1]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+2]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+3]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+4]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+5]);
p_vram_txt++;
            break;
        case CG_LINE_2:
dsp_char = 6* (vram_data[p_vram_txt]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+1]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+2]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+3]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+4]);
vram_line[p_vram_line++] = pgm_read_byte(&cg_code[dsp_char+5]);
p_vram_txt++;
            break;
        case CG_NONE:
            break;
        default:
            OutTxt_JC(UH_AddrDataNow, Mode_Line, UB_Color);
            PORTC = 0x00;
    }
}

void (CINT::*TBL_int_icf1[])(void) = {
    &CINT::Int_ICF1_RGB_TEST,
    &CINT::Int_ICF1_RGB_TEST,       
    &CINT::Int_ICF1_RGB_NUCO,       
    &CINT::Int_ICF1_RGB_NUCO,       
    &CINT::Int_ICF1_RGB_TEXT2,      
    &CINT::Int_ICF1_RGB_DRAW,           
    &CINT::Int_ICF1_Reserve,
};

void CINT::ExecFunc(uint8_t nIndex)
{   
    (this->*TBL_int_icf1[nIndex])();
};

CINT clsINT;

ISR (TIMER1_CAPT_vect)
{
    clsINT.FitDelay(TCNT1L);
    clsINT.ExecFunc(UB_ModeCRT);
}

void CINT::Int_ICF1_Reserve()
{
}

void CINT::JobAsy()
{
    static uint8_t UBCT_16_5ms = 0;
    UBCT_16_5ms++;

    if ((UBCT_16_5ms % 4) == 0)
        f_SYS_66ms_req = 1;

    if ((UBCT_16_5ms %64 ) == 0)
        f_SYS_1056ms_req = 1;
}

void CINT::Int_ICF1_RGB_NUCO()
{
    static uint8_t h_CountHpixLine = 0;
    UH_hline++;

    if (UH_hline == H_LINE_END_VGA_A)
    {
        UH_hline = 0;
        h_CountHpixLine = 0;
    }

    if ((UH_hline >= V_BLANK_VGA_A) && (UH_hline < H_LINE_FRM_VGA_NUCO))
    {
        OutData_70(UH_AddrDataNow);
        PORTC = 0x00;
        h_CountHpixLine++;

        if( h_CountHpixLine == 6 )
        {
            h_CountHpixLine = 0;
            UH_AddrDataNow += H_DOT_N_VGA_A;
        }
    }

    else if (UH_hline == V_SYNC_START_VGA_A)
    {
        P_VSYNC_OFF;                            /* start of timing of V-sync. pulse */
        JobAsy();
        UH_AddrDataNow = UH_AddrDataTop;
    }
    else if (UH_hline == V_SYNC_END_VGA_A)
    {
        P_VSYNC_ON;                             /* end of timing of V-sync pulse    */
    }
}

void CCRT::Set_Data_sin()
{
    uint16_t i;
    uint8_t j;

    i=0;

    for( i = 0; i < VRAM_SIZE; i++ )
    {
        vram_data[i] = pgm_read_byte( &VGA_SIN_TBL[ j ] );
        j++;
    }

}


void CCRT::Set_Data_1(uint16_t indexRAM, uint8_t indexROM )
{
    vram_data[indexRAM] = pgm_read_byte(&VGA_SIN_TBL[indexROM]);
}

PROGMEM const uint8_t CCRT::TEST1_data[ 64*3 ] = {
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,31,32,33,
34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
52,53,54,55,56,57,58,59,60,61,62,63,
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,31,32,33,
34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
52,53,54,55,56,57,58,59,60,61,62,63,
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,31,32,33,
34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
52,53,54,55,56,57,58,59,60,61,62,63,
};

PROGMEM const uint8_t CCRT::VGA_SIN_TBL[ 256 ] = {
102, 104, 107, 109, 112, 114, 117, 119, 121, 124, 126, 129, 131, 133,
135, 138, 140, 142, 144, 147, 149, 151, 153, 155, 157, 159, 161, 163,
165, 167, 168, 170, 172, 174, 175, 177, 178, 180, 181, 183, 184, 185,
187, 188, 189, 190, 191, 192, 193, 194, 194, 195, 196, 197, 197, 198,
198, 198, 199, 199, 199, 199,199,199, 199,199,199,199,199,198,198,198,
197, 197, 196, 195, 194,194,193,192,191,190,189,188,187,185,184,183,
181,180,178,177, 175,174,172,170,168,167,165,163,161,159,157,155,
153,151,149,147, 144,142,140,138,135,133,131,129,126,124,121,119,
117,114,112,109, 107,104,102,100,97,95,92,90,87,85,82,80,
78,75,73,70,68,66, 64,61,59,57,55,52,50,48,46,44,42,40,
38,36,34,32,31,29, 27,25,24,22,21,19,18,16,15,14,12,11,10,9,
8,7,6,5,5,4,3,3,2, 1, 1, 1, 0,0,0,0,0,0,0,0,0,0,0,1,1,1,2,2,3,4,5,
5,6,7,8,9,10,11,12,14,15,16,18,19,21,22,24,
25,27,29,31,32,34,36,38,40,42,44,46,48,50,52,55,
57,59,61,63,66,68,70,73,75,78,80,82,85,87,90,92,95,97,99
};

CCRT::CCRT()
{
    Ini();
}

void CCRT::Ini()
{
    ChangeMode(CRT_TEST1);
}

void CINT::Int_ICF1_RGB_TEST()
{
    UH_hline++;

    if (UH_hline == H_LINE_END_VGA_A)
    {
        UH_hline = 0;
        P_VSYNC_OFF;
        JobAsy();
        UH_AddrDataNow = UH_AddrDataTop;
    }
    else if (UH_hline == V_SYNC_END_VGA_A)
    {
        P_VSYNC_ON;
    }

    if ((UH_hline >= V_BLANK_VGA_A) && (UH_hline < H_LINE_FRM_VGA_A))
    {
        OutData_64(UH_AddrDataNow);
        PORTC = 0x00;

        if ((UH_hline) % 4 == 0)
            UH_AddrDataNow += 1;
    }
}


PROGMEM const uint8_t CCRT::AyaNuco1[ 70 * 64 ] = { 
63,63,63,63,63,63,0,0,63,63,63,63,63,63,63,63,63,20,0,0,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,42,0,0,0,0,24,24,24,24,24,24,24,24,24,24,24,24,24,24,
24,24,24,24,24,24,24,24,24,24,24,63,63,63,63,63,63,0,0,0,63,63,63,62,0,42,63,63,63,0,0,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,24,24,24,24,
24,24,40,24,40,40,40,40,40,40,40,40,40,24,24,
24,24,24,24,24,24,24,24,63,63,63,63,63,63,63,0,0,63,63,63,0,0,0,0,63,0,0,20,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,41,0,0,0,24,40,24,40,40,24,
24,4,0,0,0,0,0,0,0,0,0,0,0,4,24,40,24,24,24,24,24,24,24,63,63,63,63,63,63,63,
62,0,0,41,0,0,20,0,0,0,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
20,0,0,4,40,40,24,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,24,
24,24,24,24,63,63,63,63,63,63,63,63,21,0,0,0,42,63,
63,63,42,41,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,21,0,0,4,21,0,0,
0,0,0,0,0,0,42,63,63,63,63,63,63,63,0,0,5,31,31,0,0,0,24,
24,24,24,24,24,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,0,0,0,
0,25,63,63,63,63,63,63,63,63,63,63,63,0,0,0,31,31,0,0,0,4,24,
24,24,24,24,24,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,0,0,62,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,1,31,10,0,0,40,40,24,24,
24,24,24,24,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,0,0,0,62,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,0,0,0,0,0,24,40,24,24,0,0,4,24,24,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,21,0,
0,0,4,40,24,4,0,0,0,0,24,24,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,62,
62,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,24,40,0,0,0,41,0,0,0,24,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,0,0,0,0,0,62,20,0,0,25,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,4,0,0,
0,47,0,0,0,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,42,0,0,0,0,0,0,0,0,0,0,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,62,0,20,0,0,20,0,0,0,27,31,0,0,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,
0,63,47,63,47,0,0,63,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,63,
4,0,0,0,11,11,5,0,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,
42,47,63,63,63,0,0,63,0,0,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,63,
63,20,0,0,6,11,11,5,0,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,21,0,0,63,63,63,62,0,63,63,4,63,47,47,63,63,0,
0,63,0,0,0,63,63,63,63,63,63,63,63,63,63,63,0,0,62,63,63,0,1,0,0,31,27,0,0,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,63,63,63,0,0,0,
21,63,47,63,47,47,63,0,0,63,0,0,0,0,63,63,63,63,63,63,63,63,63,63,0,21,63,63,
63,0,22,0,0,0,0,0,0,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,4,0,42,63,63,63,63,0,0,63,63,47,47,63,4,0,0,
63,0,47,0,0,20,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,26,43,0,0,0,0,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,62,0,47,0,0,63,63,
63,63,62,0,0,63,63,63,42,0,0,63,0,0,47,47,0,0,21,63,63,63,63,63,63,63,63,63,63,
63,63,63,0,5,43,43,43,22,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,4,0,47,0,0,21,63,63,63,63,0,0,0,0,0,0,0,62,63,0,0,47,
31,47,0,0,62,63,63,63,63,63,63,63,63,63,63,63,63,0,5,27,27,27,27,0,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
0,4,47,47,0,0,0,0,4,41,42,4,0,0,0,0,0,42,62,0,0,31,31,31,47,
0,0,63,63,63,63,63,63,63,63,63,63,63,63,0,22,43,27,27,27,0,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,
31,31,31,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26,31,31,31,31,31,0,4,63,
63,63,63,63,63,63,63,63,63,63,0,5,27,27,27,26,0,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,0,0,47,31,31,31,31,47,47,47,47,47,31,26,26,5,
0,0,0,0,0,47,31,31,31,31,47,0,0,63,63,63,63,63,63,63,63,63,63,63,0,26,43,
27,27,22,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,
0,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,47,0,
0,63,63,63,63,63,63,63,63,63,63,0,26,27,27,27,22,0,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,0,0,31,31,31,31,31,31,31,31,31,31,31,
31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,0,0,63,63,63,63,63,63,63,63,63,63,0,27,
43,27,27,27,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,4,0,6,
31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,47,0,41,
63,63,63,63,63,63,63,63,63,0,22,27,27,27,26,0,62,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,0,0,47,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
31,31,31,31,31,31,31,31,31,47,0,0,63,63,63,63,63,63,63,63,63,0,5,27,27,27,22,0,62,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,31,31,31,31,31,
31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,47,47,47,31,31,31,47,0,0,63,63,63,63,63,
63,63,63,63,0,5,43,27,27,5,0,62,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,41,0,47,47,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,47,0,0,0,0,47,
31,31,47,0,0,63,63,63,63,63,63,63,63,63,0,1,43,27,27,1,0,62,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,0,0,47,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
31,31,31,31,31,26,0,0,0,0,0,31,31,47,0,0,63,63,63,63,63,63,63,63,63,0,1,43,27,27,22,0,
62,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,47,5,31,31,31,31,31,
31,31,31,31,31,31,31,31,31,31,31,31,31,0,0,0,0,0,0,47,31,47,26,0,42,63,63,63,63,63,63,
63,63,0,0,43,27,27,22,0,62,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
0,5,0,0,0,26,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,47,0,0,0,0,0,0,47,31,31,26,0,
62,63,63,63,63,63,63,63,63,0,0,27,27,27,22,0,62,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,62,0,42,0,0,0,0,47,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,0,0,
0,62,0,0,26,31,31,31,0,62,63,63,63,63,63,63,63,63,0,0,43,27,27,22,0,42,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,4,0,47,0,0,0,0,0,47,31,31,31,31,31,31,31,31,
31,31,31,31,31,31,4,0,0,62,0,0,26,31,47,26,0,63,63,63,63,63,63,63,63,63,0,0,43,27,27,
22,0,62,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,27,0,42,0,0,0,47,31,
31,31,31,31,31,31,31,31,31,31,31,31,31,47,0,0,0,0,0,31,31,47,5,0,63,63,63,63,63,63,63,
63,63,0,0,43,27,27,22,0,42,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,
4,0,63,0,0,0,47,31,31,31,31,31,47,47,31,31,31,31,31,31,31,47,0,0,0,0,0,47,31,47,0,0,63,
63,63,63,63,63,63,63,63,0,0,27,27,27,26,0,41,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,0,0,0,0,0,0,0,0,47,31,31,31,31,0,5,47,0,47,10,31,31,31,31,31,26,0,0,0,0,31,
47,47,0,0,63,63,63,63,63,63,63,63,63,0,0,43,27,27,27,0,42,63,63,63,63,63,63,63,63,63,
63,63,63,63,62,0,0,0,0,0,0,0,0,0,0,0,0,0,47,31,31,31,47,0,0,5,0,5,0,0,47,31,31,31,
31,5,0,0,47,47,47,0,0,42,63,63,63,63,63,63,63,63,63,0,0,43,27,27,27,0,21,63,63,63,63,
63,63,63,63,63,63,63,42,0,0,0,0,0,0,0,0,0,47,0,0,0,0,0,47,31,31,31,31,0,0,0,0,0,0,0,
47,31,31,31,31,31,47,47,47,47,0,0,0,63,63,63,63,63,63,63,63,63,63,
0,0,43,27,27,27,0,4,63,63,63,63,63,63,63,63,63,63,63,0,0,0,42,62,21,0,0,0,0,0,0,0,0,
0,26,31,31,31,31,31,5,0,0,0,0,0,10,31,31,31,31,31,31,31,47,47,0,0,0,63,63,63,63,63,63,
63,63,63,63,63,0,0,43,27,27,43,0,0,63,63,63,63,63,63,63,63,63,63,63,0,63,63,63,63,63,
63,63,20,0,0,0,0,0,0,47,47,31,31,31,31,47,47,47,47,47,47,31,31,31,31,31,31,47,47,26,
0,0,20,63,63,63,63,63,63,63,63,63,63,63,63,0,0,43,27,27,27,0,0,63,63,63,63,63,
63,63,63,63,63,62,0,63,63,63,63,63,63,63,63,0,0,22,0,0,0,0,0,47,31,31,31,31,31,31,
31,31,31,31,31,31,31,47,47,0,0,0,0,62,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,
43,27,27,43,0,0,63,63,63,63,63,63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,0,0,
0,63,47,0,0,0,0,4,47,31,31,31,31,31,31,31,47,31,47,31,0,0,0,0,0,4,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,0,0,43,27,27,43,0,0,63,63,63,63,63,63,63,63,63,63,63,0,
63,63,63,63,63,63,63,63,63,0,0,0,22,47,21,0,0,0,0,47,47,47,47,47,47,26,4,0,0,0,0,
0,31,47,0,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,27,27,27,43,0,0,63,63,63,63,63,
63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,26,47,47,31,31,47,0,0,0,63,63,63,63,63,63,63,63,63,63,63,63,0,0,43,27,27,43,0,0,
63,63,63,63,63,63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
0,0,0,0,0,0,0,0,6,31,47,47,47,31,31,31,31,31,31,31,31,47,0,0,20,63,63,63,63,63,63,
63,63,63,63,63,0,0,43,27,27,43,0,0,63,63,63,63,63,63,63,63,63,63,63,0,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,0,0,0,31,31,31,31,31,31,31,
31,31,31,47,0,0,63,63,63,63,63,63,63,63,63,63,63,0,0,43,27,27,43,0,0,63,63,63,63,63,63,63,63,
63,63,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,0,26,31,31,
31,31,31,31,31,31,31,31,47,0,0,63,63,63,63,63,63,63,63,63,63,63,0,0,43,27,27,43,0,0,63,63,
63,63,63,63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
0,0,0,0,0,47,31,31,31,31,31,31,31,31,31,31,47,26,0,62,63,63,63,63,63,63,63,63,63,63,0,0,
43,27,27,43,0,0,63,63,63,63,63,63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,0,0,0,0,0,31,31,31,31,31,31,31,31,31,31,31,47,26,0,62,63,63,63,63,
63,63,63,63,63,63,0,0,43,27,27,43,0,0,63,63,63,63,63,63,63,63,63,63,63,0,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,4,47,47,31,31,31,31,31,31,31,31,
31,31,47,4,0,63,21,5,0,25,63,63,63,63,63,63,0,5,43,27,27,43,0,0,63,63,63,63,63,63,63,
63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,31,
31,31,31,31,31,31,31,31,31,31,31,47,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,47,43,43,47,1,0,
63,63,63,63,63,63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,20,0,0,47,31,31,31,31,31,31,31,31,31,47,47,0,0,0,1,26,27,6,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,63,63,63,63,63,63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,4,0,0,47,31,31,31,31,31,47,47,47,47,0,0,0,27,
11,11,11,11,11,11,11,27,11,6,0,0,0,0,0,0,0,0,63,63,63,63,63,63,63,63,63,63,63,0,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,47,47,
47,31,0,0,0,0,0,0,0,27,11,11,11,11,11,11,11,11,11,11,11,27,27,27,27,27,27,0,0,63,
63,63,63,63,63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,6,63,63,63,63,63,63,63,63,63,63,63,0,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,0,6,0,0,0,0,0,0,5,
31,26,6,27,27,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,63,63,63,63,63,63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,0,0,0,31,27,27,11,27,11,31,11,6,27,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,63,63,63,63,63,
63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
0,0,27,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,63,63,63,63,
63,63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,25,0,
0,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,63,63,63,63,63,63,63,63,63,63,63,0,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,27,
27,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,27,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,63,63,63,63,63,63,63,63,63,63,63,0,
63,63,63,63,63,63,62,62,62,20,0,0,0,20,20,21,
42,0,0,47,31,27,27,11,11,11,11,11,11,11,11,11,27,27,27,27,27,27,26,5,5,5,5,22,
27,27,27,27,27,27,27,27,27,27,27,27,27,27,41,63,63,63,63,63,63,63,63,63,63,
0,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,11,27,27,11,11,
6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,20,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,41,41,
41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,20,0,0,0,0,0,0,0,0,0,0,0,
21,21,41,41,41,41,41,41,41,41,41,41,21,0,0,20,20,21,41,41,41,41,41,41,41,
};

PROGMEM const uint8_t CCRT::AyaNuco2[ 70 * 64 ] = { 
63,63,63,63,63,63,0,0,
63,63,63,63,63,63,63,63,63,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,0,0,0,0,24,24,24,24,24,24,24,24,24,24,24,41,24,24,24,24,24,24,24,24,24,24,24,24,
24,24,24,24,63,63,63,63,63,63,0,0,0,63,63,20,0,0,21,63,63,0,0,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,42,0,0,0,42,24,41,24,41,24,41,24,24,4,4,4,0,0,4,20,24,24,
24,41,24,24,24,24,24,24,24,24,24,24,63,63,63,63,63,63,63,0,0,63,21,0,0,0,0,0,0,0,25,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,20,41,41,24,24,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,24,24,24,24,24,24,24,24,24,63,63,63,63,63,63,63,63,0,0,0,0,63,63,
25,0,0,21,63,63,63,63,63,63,63,62,63,63,63,63,63,63,63,63,0,0,0,25,24,0,0,0,0,0,0,0,0,
20,41,63,63,63,63,63,0,0,0,5,0,0,0,0,24,24,24,24,24,24,24,24,63,63,63,63,63,63,63,63,
63,21,21,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,0,41,63,63,21,0,0,0,0,0,0,
0,0,20,63,63,63,63,63,63,63,63,63,63,0,0,5,31,31,0,0,0,24,24,24,24,24,24,24,24,24,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,25,0,63,63,63,
63,63,63,0,0,0,0,0,0,62,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,31,31,1,0,0,41,24,
24,24,24,24,24,24,24,24,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,25,0,0,41,63,63,63,63,63,63,0,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,0,0,0,5,0,0,41,24,41,20,0,0,41,24,24,24,24,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,0,0,0,0,63,63,63,63,63,63,63,0,0,0,0,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,24,41,41,0,0,0,0,24,24,24,24,24,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,63,63,63,63,63,41,0,0,63,62,
0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,41,20,0,0,0,26,0,0,24,
24,24,24,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,
63,63,63,0,0,62,63,0,47,21,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,
20,0,0,0,26,0,0,0,0,0,24,24,24,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,0,0,0,42,63,63,0,42,63,0,24,63,0,0,0,0,0,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,0,0,0,0,0,0,26,0,5,31,6,0,0,24,24,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,20,0,63,63,0,0,63,63,47,63,63,0,0,0,0,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,0,0,0,0,63,0,0,0,31,11,10,0,0,20,20,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,63,63,0,0,63,47,63,47,63,63,
62,0,0,0,0,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,63,63,0,0,0,31,11,10,0,0,0,0,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,63,63,63,63,0,
0,63,47,63,47,63,63,0,0,63,0,0,0,63,63,63,63,63,63,63,63,63,63,63,0,0,63,63,63,0,0,0,31,
31,1,0,63,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,
0,63,63,63,63,0,0,63,63,63,63,63,0,0,25,63,0,0,0,0,63,63,63,63,63,63,63,63,63,63,0,63,63,
63,63,0,26,0,0,1,0,0,63,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,0,21,0,63,63,63,63,63,0,0,0,25,0,0,0,0,63,63,0,5,26,0,0,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,0,47,5,0,0,0,26,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,25,0,63,0,0,63,63,63,63,63,0,0,0,0,0,20,63,63,42,0,47,47,5,0,0,63,
63,63,63,63,63,63,63,63,63,63,63,42,0,43,43,43,43,0,42,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,47,0,0,42,63,63,63,63,63,0,0,63,63,63,63,
63,0,0,47,31,47,0,0,25,63,63,63,63,63,63,63,63,63,63,63,25,0,43,27,27,43,0,42,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,26,47,47,0,0,0,0,0,
0,0,0,0,63,63,63,63,63,0,0,31,31,31,47,0,0,63,63,63,63,63,63,63,63,63,63,63,20,0,43,27,
27,43,0,42,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
0,47,31,31,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,31,31,31,31,47,0,0,63,63,63,63,63,63,63,63,
63,63,0,0,43,27,27,43,0,42,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,0,0,47,31,31,31,31,31,47,31,47,31,31,31,31,31,26,5,5,0,31,31,31,31,31,31,0,
0,63,63,63,63,63,63,63,63,63,63,0,0,43,27,27,43,0,42,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,47,31,31,31,31,31,31,31,31,31,31,31,31,31,
31,31,31,31,31,31,31,31,31,31,47,0,0,63,63,63,63,63,63,63,63,63,21,0,43,27,27,43,0,26,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,5,31,31,31,
31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,47,5,0,63,63,63,63,63,63,
63,63,63,20,0,43,27,27,43,0,21,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,0,0,26,31,31,31,31,31,31,5,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
31,31,47,0,42,63,63,63,63,63,63,63,63,20,0,43,27,27,43,0,0,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,31,31,31,31,31,31,47,0,47,0,31,31,0,0,0,31,
31,31,31,31,47,47,31,31,31,31,47,0,0,63,63,63,63,63,63,63,63,20,0,43,27,27,43,0,0,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,47,31,31,31,31,
31,31,0,47,0,10,31,31,26,0,31,31,31,31,26,0,0,10,47,31,31,47,0,0,63,63,63,63,63,63,63,63,
21,0,43,27,27,43,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,0,0,
47,47,31,31,31,31,31,31,10,31,31,31,31,31,31,31,31,31,31,47,0,0,0,0,26,31,31,47,0,0,63,
63,63,63,63,63,63,63,25,0,43,27,27,43,0,20,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,0,0,63,63,21,0,0,0,0,47,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,47,0,0,0,
0,0,47,31,47,0,0,63,63,63,63,63,63,63,63,42,0,43,27,27,43,0,0,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,63,0,0,0,63,63,63,63,0,0,0,0,0,47,31,31,31,31,31,31,31,31,31,31,31,31,
31,31,31,26,0,0,0,0,0,31,31,47,5,0,63,63,63,63,63,63,63,63,21,0,43,27,27,43,0,0,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,0,0,63,63,63,63,63,63,0,21,0,0,0,0,47,31,31,31,31,31,31,
31,31,31,31,31,31,31,31,5,0,0,63,0,0,31,31,31,5,0,63,63,63,63,63,63,63,63,21,0,43,27,27,
43,0,0,63,63,63,63,63,63,63,63,63,63,63,63,62,0,0,63,63,63,63,63,63,62,0,47,0,0,0,0,0,47,
31,31,31,31,31,31,31,31,31,31,31,31,31,26,0,0,63,0,0,10,47,47,5,0,63,63,63,63,63,63,63,
63,42,0,43,27,27,43,0,20,63,63,63,63,63,63,63,63,63,63,63,63,0,25,63,63,63,63,63,63,63,
21,0,47,0,0,0,0,0,0,47,31,31,31,31,31,31,31,31,31,31,31,31,47,0,0,0,0,0,26,31,47,5,0,63,
63,63,63,63,63,63,63,25,0,43,27,27,43,0,0,63,63,63,63,63,63,63,63,63,63,63,0,0,63,63,63,
63,63,63,63,63,0,0,21,0,63,0,0,0,0,47,31,31,31,31,31,31,31,47,31,31,31,31,31,1,0,0,0,0,
47,31,47,0,0,63,63,63,63,63,63,63,63,42,0,43,27,27,43,0,0,63,63,63,63,63,63,63,63,63,63,
0,0,63,63,63,63,63,63,63,63,63,0,0,0,0,63,0,0,0,0,0,47,31,0,47,31,47,0,47,47,0,31,31,31,
31,0,0,0,0,31,47,47,0,0,63,63,63,63,63,63,63,63,42,0,43,27,27,43,0,0,63,63,63,63,63,63,
63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,0,0,0,0,0,0,0,0,0,0,0,47,0,47,47,10,0,47,
47,0,31,31,31,31,0,0,0,47,47,47,0,0,62,63,63,63,63,63,63,63,63,42,0,43,27,27,43,0,0,63,
63,63,63,63,63,63,63,63,0,0,63,63,63,63,63,63,63,63,63,63,0,0,26,0,0,0,0,0,47,0,0,47,
0,0,10,0,0,0,0,0,47,31,31,31,47,47,47,47,47,0,0,0,63,63,63,63,63,63,63,63,63,62,0,43,
27,27,43,0,0,63,63,63,63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,63,0,0,47,47,0,
0,0,31,47,0,0,0,47,0,0,0,47,47,47,0,31,31,31,31,31,31,31,47,0,0,0,63,63,63,63,63,63,
63,63,63,63,63,0,43,27,27,43,0,0,63,63,63,63,63,63,63,63,0,0,63,63,63,63,63,63,63,63,63,
63,63,62,0,47,31,47,47,47,47,31,47,0,0,47,47,47,0,0,47,47,0,31,31,31,31,31,47,47,0,0,
0,63,63,63,63,63,63,63,63,63,63,63,42,0,43,27,27,43,0,0,63,63,63,63,63,63,63,63,0,42,
63,63,63,63,63,63,63,63,63,63,63,63,0,0,47,47,31,31,31,31,31,5,0,0,47,31,47,0,0,0,0,
31,31,47,47,0,0,0,0,42,63,63,63,63,63,63,63,63,63,63,63,63,25,0,43,27,27,43,0,0,63,63,
63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,47,47,31,31,31,31,47,47,
0,0,47,47,47,47,0,26,47,47,47,0,0,0,0,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,42,0,43,
27,27,43,0,0,63,63,63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,
5,31,47,47,47,47,1,0,0,47,47,47,47,5,0,0,0,0,0,31,47,0,0,0,63,63,63,63,63,63,63,63,63,
63,63,63,21,0,43,27,27,43,1,0,63,63,63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26,47,31,31,31,47,5,0,0,63,63,63,63,
63,63,63,63,63,63,63,21,0,43,27,27,43,0,0,63,63,63,63,63,63,63,63,20,0,63,63,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,21,0,0,0,0,0,0,0,47,47,47,47,31,31,31,31,31,31,
31,31,47,1,0,21,63,63,63,63,63,63,63,63,63,63,21,0,43,27,27,43,0,0,63,63,63,63,63,63,63,
63,63,0,0,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,
0,31,31,31,31,31,31,31,31,31,31,47,0,0,63,63,63,63,63,63,63,63,63,63,21,0,43,27,27,43,
1,0,63,63,63,63,63,63,63,63,63,63,0,0,0,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,0,0,0,0,5,31,31,31,31,31,31,31,31,31,31,47,0,0,63,63,63,63,63,63,63,63,
63,63,21,0,43,27,27,43,0,0,63,63,63,63,63,63,63,63,63,63,0,63,63,63,0,0,0,21,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,0,31,31,31,31,31,31,31,31,31,31,47,26,0,
63,63,63,63,63,63,63,63,63,63,20,0,43,27,27,27,1,0,63,63,63,63,63,63,63,63,63,0,0,63,63,
63,63,63,0,0,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,5,31,31,31,31,31,31,31,
31,31,31,31,47,5,0,63,63,63,63,63,63,63,63,63,63,0,1,43,27,27,43,5,0,21,63,63,63,63,63,
63,63,63,0,63,63,63,63,63,63,63,63,63,0,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,
47,47,31,31,31,31,31,31,31,31,31,31,47,0,0,21,0,0,0,0,21,42,63,63,63,21,0,43,43,27,43,
21,0,20,63,63,63,63,63,63,63,21,0,63,63,63,63,63,63,63,63,63,63,63,0,0,0,63,63,63,63,63,
63,63,63,63,63,63,21,0,0,47,31,31,31,31,31,31,31,31,31,31,47,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,27,27,43,43,43,0,0,63,63,63,63,63,63,63,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,0,
0,63,63,63,63,63,63,63,63,63,63,63,0,0,0,47,31,31,31,31,31,31,47,31,47,27,0,0,1,27,27,
27,27,27,6,6,0,0,0,0,0,0,0,0,0,0,0,63,63,63,63,63,63,63,0,63,63,63,63,63,63,63,63,63,
63,63,63,63,63,63,0,0,0,63,63,63,63,63,63,63,63,63,63,0,0,0,47,31,31,31,47,0,5,47,5,0,
0,6,27,11,11,11,11,11,11,11,11,27,11,6,6,6,1,0,0,0,0,63,63,63,63,63,63,21,0,63,63,63,
63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,63,63,63,63,63,63,63,26,0,0,0,0,0,47,47,
1,0,0,0,0,0,0,1,27,11,11,11,11,11,11,11,11,11,11,11,11,11,27,27,27,27,0,0,0,42,63,63,
63,63,0,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,63,63,63,63,0,0,0,
0,0,0,0,0,0,0,0,0,1,0,0,0,5,27,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
27,1,0,0,63,63,63,63,0,63,63,63,63,63,63,63,63,63,63,63,63,63,63,0,0,0,0,0,0,0,
41,63,63,0,0,0,27,27,27,27,27,5,0,0,0,27,27,27,27,27,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,27,10,0,63,63,63,63,0,63,63,63,63,63,63,63,63,
63,63,63,25,0,0,0,21,63,0,0,63,63,63,0,0,0,0,0,47,27,27,11,11,11,27,27,27,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,63,63,
63,63,0,62,63,63,63,63,63,63,63,63,42,0,0,0,63,63,0,0,63,0,0,63,63,63,21,0,0,
0,0,0,27,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,63,63,63,63,0,0,63,63,63,63,63,63,42,0,0,0,63,63,
63,63,0,0,0,63,0,0,0,63,0,0,0,0,63,0,0,27,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,63,63,63,63,0,0,63,
63,63,63,42,0,0,0,0,0,0,63,63,63,63,21,0,0,63,42,0,0,0,0,63,0,0,63,0,0,27,11,11,11,
11,11,11,11,27,11,27,27,27,27,27,27,27,27,27,27,27,11,11,11,11,27,11,27,27,27,27,27,27,
27,63,63,63,63,63,0,63,63,63,0,0,0,63,63,0,0,0,42,63,63,63,63,21,63,63,63,63,0,0,0,
0,0,0,63,63,0,31,27,27,27,27,27,5,5,5,0,0,0,0,0,0,0,0,0,0,1,1,6,10,27,5,1,1,0,0,0,0,
0,0,0,0,0,63,42,41,0,0,0,0,0,0,0,0,63,63,63,0,0,62,63,63,63,63,0,0,63,63,63,0,0,0,0,0,
0,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,25,63,63,63,63,0,0,0,0,0,63,63,63,63,63,0,0,20,0,0,63,0,0,0,0,0,0,
0,0,0,0,20,20,36,36,20,41,41,57,20,0,0,0,0,0,0,0,20,20,36,36,20,20,20,40,40,40,20,
20,20,36,41,36,0,0,0,40,40,41,0,0,0,63,63,63,63,0,63,63,0,0,0,63,63,63,63,0,21,0,0,
63,0,0,0,36,36,40,57,40,57,57,57,57,40,0,0,0,0,0,0,0,0,0,57,57,56,56,56,56,56,56,
56,56,56,56,56,40,40,40,40,40,40,40,40,40,40,40,40,40,40,57,0,0,63,63,63,0,0,0,0,0,
0,63,63,63,63,20,0,0,63,21,0,57,40,40,40,40,40,56,41,20,0,0,0,0,42,63,63,63,63,63,
0,0,0,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,0,0,40,40,40,40,40,40,40,40,
40,40,40,0,0,63,63,63,0,63,63,63,63,63,63,63,63,63,0,0,63,0,0,40,40,40,40,40,
40,0,0,0,0,63,63,63,63,42,0,0,0,0,0,20,20,56,40,40,40,40,40,40,40,40,40,40,40,
};

void CINT::OutDraw(uint8_t Delay, uint8_t Color1, uint8_t Color2)
{
    __asm__ __volatile("\n"
        "mov r30,%0\n\t"
        "ldi r31,0x00\n\t"
        "mov r20,%1\n\t"
        "mov r21,%2\n\t"
        "addr_top = . +14\n\t"
        "ldi r24,lo8(addr_top)\n\t" //  2byte   1cycle
        "ldi r25,hi8(addr_top)\n\t" //  2byte   1cycle
        "lsr r24\n\t"               //  2byte   1cycle
        "ror r25\n\t"               //  2byte   1cycle
        "add r30,r24\n\t"           //  2byte   1cycle
        "adc r31,r25\n\t"           //  2byte   1cycle
        "ijmp\n\t"                  //  2byte   2cycle

        "nop\n\t"               //0
        "nop\n\t"               //1
        "nop\n\t"               //2
        "nop\n\t"               //3
        "nop\n\t"               //4
        "nop\n\t"               //5
        "nop\n\t"               //6
        "nop\n\t"               //7
        "nop\n\t"               //8
        "nop\n\t"               //9
        "nop\n\t"               //10
        "nop\n\t"               //11
        "nop\n\t"               //12
        "nop\n\t"               //13
        "nop\n\t"               //14
        "nop\n\t"               //15
        "nop\n\t"               //16
        "nop\n\t"               //17
        "nop\n\t"               //18
        "nop\n\t"               //19
        "nop\n\t"               //20
        "nop\n\t"               //21
        "nop\n\t"               //22
        "nop\n\t"               //23
        "nop\n\t"               //24
        "nop\n\t"               //25
        "nop\n\t"               //26
        "nop\n\t"               //27
        "nop\n\t"               //28
        "nop\n\t"               //29
        "nop\n\t"               //30
        "nop\n\t"               //31
        "nop\n\t"               //32
        "nop\n\t"               //33
        "nop\n\t"               //34
        "nop\n\t"               //35
        "nop\n\t"               //36
        "nop\n\t"               //37
        "nop\n\t"               //38
        "nop\n\t"               //39
        "nop\n\t"               //40
        "nop\n\t"               //41
        "nop\n\t"               //42
        "nop\n\t"               //43
        "nop\n\t"               //44
        "nop\n\t"               //45
        "nop\n\t"               //46
        "nop\n\t"               //47
        "nop\n\t"               //48
        "nop\n\t"               //49
        "nop\n\t"               //50
        "nop\n\t"               //51
        "nop\n\t"               //52
        "nop\n\t"               //53
        "nop\n\t"               //54
        "nop\n\t"               //55
        "nop\n\t"               //56
        "nop\n\t"               //57
        "nop\n\t"               //58
        "nop\n\t"               //59
        "nop\n\t"               //60
        "nop\n\t"               //61
        "nop\n\t"               //62
        "nop\n\t"               //63
        "nop\n\t"               //64
        "nop\n\t"               //65
        "nop\n\t"               //66
        "nop\n\t"               //67
        "nop\n\t"               //68
        "nop\n\t"               //69
        "nop\n\t"               //70
        "nop\n\t"               //71
        "nop\n\t"               //72
        "nop\n\t"               //73
        "nop\n\t"               //74
        "nop\n\t"               //75
        "nop\n\t"               //76
        "nop\n\t"               //77
        "nop\n\t"               //78
        "nop\n\t"               //79
        "nop\n\t"               //80
        "nop\n\t"               //81
        "nop\n\t"               //82
        "nop\n\t"               //83
        "nop\n\t"               //84
        "nop\n\t"               //85
        "nop\n\t"               //86
        "nop\n\t"               //87
        "nop\n\t"               //88
        "nop\n\t"               //89
        "nop\n\t"               //90
        "nop\n\t"               //91
        "nop\n\t"               //92
        "nop\n\t"               //93
        "nop\n\t"               //94
        "nop\n\t"               //95
        "nop\n\t"               //96
        "nop\n\t"               //97
        "nop\n\t"               //98
        "nop\n\t"               //99

        "nop\n\t"               //0 /100
        "nop\n\t"               //1
        "nop\n\t"               //2
        "nop\n\t"               //3
        "nop\n\t"               //4
        "nop\n\t"               //5
        "nop\n\t"               //6
        "nop\n\t"               //7
        "nop\n\t"               //8
        "nop\n\t"               //9
        "nop\n\t"               //10
        "nop\n\t"               //11
        "nop\n\t"               //12
        "nop\n\t"               //13
        "nop\n\t"               //14
        "nop\n\t"               //15
        "nop\n\t"               //16
        "nop\n\t"               //17
        "nop\n\t"               //18
        "nop\n\t"               //19
        "nop\n\t"               //20
        "nop\n\t"               //21
        "nop\n\t"               //22
        "nop\n\t"               //23
        "nop\n\t"               //24
        "nop\n\t"               //25
        "nop\n\t"               //26
        "nop\n\t"               //27
        "nop\n\t"               //28
        "nop\n\t"               //29
        "nop\n\t"               //30
        "nop\n\t"               //31
        "nop\n\t"               //32
        "nop\n\t"               //33
        "nop\n\t"               //34
        "nop\n\t"               //35
        "nop\n\t"               //36
        "nop\n\t"               //37
        "nop\n\t"               //38
        "nop\n\t"               //39
        "nop\n\t"               //40
        "nop\n\t"               //41
        "nop\n\t"               //42
        "nop\n\t"               //43
        "nop\n\t"               //44
        "nop\n\t"               //45
        "nop\n\t"               //46
        "nop\n\t"               //47
        "nop\n\t"               //48
        "nop\n\t"               //49
        "nop\n\t"               //50
        "nop\n\t"               //51
        "nop\n\t"               //52
        "nop\n\t"               //53
        "nop\n\t"               //54
        "nop\n\t"               //55
        "nop\n\t"               //56
        "nop\n\t"               //57
        "nop\n\t"               //58
        "nop\n\t"               //59
        "nop\n\t"               //60
        "nop\n\t"               //61
        "nop\n\t"               //62
        "nop\n\t"               //63
        "nop\n\t"               //64
        "nop\n\t"               //65
        "nop\n\t"               //66
        "nop\n\t"               //67
        "nop\n\t"               //68
        "nop\n\t"               //69
        "nop\n\t"               //70
        "nop\n\t"               //71
        "nop\n\t"               //72
        "nop\n\t"               //73
        "nop\n\t"               //74
        "nop\n\t"               //75
        "nop\n\t"               //76
        "nop\n\t"               //77
        "nop\n\t"               //78
        "nop\n\t"               //79
        "nop\n\t"               //80
        "nop\n\t"               //81
        "nop\n\t"               //82
        "nop\n\t"               //83
        "nop\n\t"               //84
        "nop\n\t"               //85
        "nop\n\t"               //86
        "nop\n\t"               //87
        "nop\n\t"               //88
        "nop\n\t"               //89
        "nop\n\t"               //90
        "nop\n\t"               //91
        "nop\n\t"               //92
        "nop\n\t"               //93
        "nop\n\t"               //94
        "nop\n\t"               //95
        "nop\n\t"               //96
        "nop\n\t"               //97
        "nop\n\t"               //98
        "nop\n\t"               //99

        "out 0x08,r20\n\t"
        "out 0x08,r21\n\t"

        :
        : "r"(Delay) ,"r"(Color1) ,"r"(Color2)
        :
    );
}

void CINT::FitDelay(uint8_t UBdelay_time)
{

    __asm__ __volatile__(       
        "cpi    %0,49\n\t"      
        "breq   L49\n\t"        
        "cpi    %0,48\n\t"      
        "breq   L48\n\t"        
        "cpi    %0,47\n\t"      
        "breq   L47\n\t"        
        "cpi    %0,46\n\t"      
        "breq   L46\n\t"        
        "cpi    %0,45\n\t"      
        "breq   L45\n\t"        
        "cpi    %0,44\n\t"      
        "breq   L44\n\t"        
        "cpi    %0,43\n\t"      
        "breq   L43\n\t"        
        "cpi    %0,42\n\t"      
        "breq   L42\n\t"        

        "L49:\n\t"
        "nop\n\t"               
        "L48:\n\t"              
        "nop\n\t"               
        "L47:\n\t"              
        "nop\n\t"               
        "L46:\n\t"              
        "nop\n\t"               
        "L45:\n\t"              
        "nop\n\t"               
        "L44:\n\t"              
        "nop\n\t"               
        "L43:\n\t"              
        "nop\n\t"               
        "L42:\n\t"              
        "nop\n\t"               
                                

        :
        : "r"(UBdelay_time)     
        :

    );
}

void CINT::OutDelay(uint16_t UH_Delay)
{
    __asm__ __volatile("\n"
        "addr_top = . +14\n\t"
        "ldi r24,lo8(addr_top)\n\t" //  2byte   1cycle
        "ldi r25,hi8(addr_top)\n\t" //  2byte   1cycle
        "lsr r24\n\t"               //  2byte   1cycle
        "ror r25\n\t"               //  2byte   1cycle
        "add r30,r24\n\t"           //  2byte   1cycle
        "adc r31,r25\n\t"           //  2byte   1cycle
        "ijmp\n\t"                  //  2byte   2cycle

        "nop\n\t"               //0
        "nop\n\t"               //1
        "nop\n\t"               //2
        "nop\n\t"               //3
        "nop\n\t"               //4
        "nop\n\t"               //5
        "nop\n\t"               //6
        "nop\n\t"               //7
        "nop\n\t"               //8
        "nop\n\t"               //9
        "nop\n\t"               //10
        "nop\n\t"               //11
        "nop\n\t"               //12
        "nop\n\t"               //13
        "nop\n\t"               //14
        "nop\n\t"               //15
        "nop\n\t"               //16
        "nop\n\t"               //17
        "nop\n\t"               //18
        "nop\n\t"               //19
        "nop\n\t"               //20
        "nop\n\t"               //21
        "nop\n\t"               //22
        "nop\n\t"               //23
        "nop\n\t"               //24
        "nop\n\t"               //25
        "nop\n\t"               //26
        "nop\n\t"               //27
        "nop\n\t"               //28
        "nop\n\t"               //29
        "nop\n\t"               //30
        "nop\n\t"               //31
        "nop\n\t"               //32
        "nop\n\t"               //33
        "nop\n\t"               //34
        "nop\n\t"               //35
        "nop\n\t"               //36
        "nop\n\t"               //37
        "nop\n\t"               //38
        "nop\n\t"               //39
        "nop\n\t"               //40
        "nop\n\t"               //41
        "nop\n\t"               //42
        "nop\n\t"               //43
        "nop\n\t"               //44
        "nop\n\t"               //45
        "nop\n\t"               //46
        "nop\n\t"               //47
        "nop\n\t"               //48
        "nop\n\t"               //49
        "nop\n\t"               //50
        "nop\n\t"               //51
        "nop\n\t"               //52
        "nop\n\t"               //53
        "nop\n\t"               //54
        "nop\n\t"               //55
        "nop\n\t"               //56
        "nop\n\t"               //57
        "nop\n\t"               //58
        "nop\n\t"               //59
        "nop\n\t"               //60
        "nop\n\t"               //61
        "nop\n\t"               //62
        "nop\n\t"               //63
        "nop\n\t"               //64
        "nop\n\t"               //65
        "nop\n\t"               //66
        "nop\n\t"               //67
        "nop\n\t"               //68
        "nop\n\t"               //69
        "nop\n\t"               //70
        "nop\n\t"               //71
        "nop\n\t"               //72
        "nop\n\t"               //73
        "nop\n\t"               //74
        "nop\n\t"               //75
        "nop\n\t"               //76
        "nop\n\t"               //77
        "nop\n\t"               //78
        "nop\n\t"               //79
        "nop\n\t"               //80
        "nop\n\t"               //81
        "nop\n\t"               //82
        "nop\n\t"               //83
        "nop\n\t"               //84
        "nop\n\t"               //85
        "nop\n\t"               //86
        "nop\n\t"               //87
        "nop\n\t"               //88
        "nop\n\t"               //89
        "nop\n\t"               //90
        "nop\n\t"               //91
        "nop\n\t"               //92
        "nop\n\t"               //93
        "nop\n\t"               //94
        "nop\n\t"               //95
        "nop\n\t"               //96
        "nop\n\t"               //97
        "nop\n\t"               //98
        "nop\n\t"               //99

        "nop\n\t"               //0 /100
        "nop\n\t"               //1
        "nop\n\t"               //2
        "nop\n\t"               //3
        "nop\n\t"               //4
        "nop\n\t"               //5
        "nop\n\t"               //6
        "nop\n\t"               //7
        "nop\n\t"               //8
        "nop\n\t"               //9
        "nop\n\t"               //10
        "nop\n\t"               //11
        "nop\n\t"               //12
        "nop\n\t"               //13
        "nop\n\t"               //14
        "nop\n\t"               //15
        "nop\n\t"               //16
        "nop\n\t"               //17
        "nop\n\t"               //18
        "nop\n\t"               //19
        "nop\n\t"               //20
        "nop\n\t"               //21
        "nop\n\t"               //22
        "nop\n\t"               //23
        "nop\n\t"               //24
        "nop\n\t"               //25
        "nop\n\t"               //26
        "nop\n\t"               //27
        "nop\n\t"               //28
        "nop\n\t"               //29
        "nop\n\t"               //30
        "nop\n\t"               //31
        "nop\n\t"               //32
        "nop\n\t"               //33
        "nop\n\t"               //34
        "nop\n\t"               //35
        "nop\n\t"               //36
        "nop\n\t"               //37
        "nop\n\t"               //38
        "nop\n\t"               //39
        "nop\n\t"               //40
        "nop\n\t"               //41
        "nop\n\t"               //42
        "nop\n\t"               //43
        "nop\n\t"               //44
        "nop\n\t"               //45
        "nop\n\t"               //46
        "nop\n\t"               //47
        "nop\n\t"               //48
        "nop\n\t"               //49
        "nop\n\t"               //50
        "nop\n\t"               //51
        "nop\n\t"               //52
        "nop\n\t"               //53
        "nop\n\t"               //54
        "nop\n\t"               //55
        "nop\n\t"               //56
        "nop\n\t"               //57
        "nop\n\t"               //58
        "nop\n\t"               //59
        "nop\n\t"               //60
        "nop\n\t"               //61
        "nop\n\t"               //62
        "nop\n\t"               //63
        "nop\n\t"               //64
        "nop\n\t"               //65
        "nop\n\t"               //66
        "nop\n\t"               //67
        "nop\n\t"               //68
        "nop\n\t"               //69
        "nop\n\t"               //70
        "nop\n\t"               //71
        "nop\n\t"               //72
        "nop\n\t"               //73
        "nop\n\t"               //74
        "nop\n\t"               //75
        "nop\n\t"               //76
        "nop\n\t"               //77
        "nop\n\t"               //78
        "nop\n\t"               //79
        "nop\n\t"               //80
        "nop\n\t"               //81
        "nop\n\t"               //82
        "nop\n\t"               //83
        "nop\n\t"               //84
        "nop\n\t"               //85
        "nop\n\t"               //86
        "nop\n\t"               //87
        "nop\n\t"               //88
        "nop\n\t"               //89
        "nop\n\t"               //90
        "nop\n\t"               //91
        "nop\n\t"               //92
        "nop\n\t"               //93
        "nop\n\t"               //94
        "nop\n\t"               //95
        "nop\n\t"               //96
        "nop\n\t"               //97
        "nop\n\t"               //98
        "nop\n\t"               //99

        :
        : "z"(UH_Delay)
        :
    );
}

void CINT::OutTxt_JC(uint16_t AddrGRP, uint8_t MaskData, uint8_t Color)
{
    __asm__ __volatile("\n\t"

        "mov r21,%2\n\t"

        "ld     r0,x+\n\t"      //1     -1
        "and    r0,%1\n\t"
        "breq   1f\n\t"
        "mov    r0,r21\n\t"
        "1:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //2
        "and    r0,%1\n\t"
        "breq   2f\n\t"
        "mov    r0,r21\n\t"
        "2:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //3
        "and    r0,%1\n\t"
        "breq   3f\n\t"
        "mov    r0,r21\n\t"
        "3:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //4
        "and    r0,%1\n\t"
        "breq   4f\n\t"
        "mov    r0,r21\n\t"
        "4:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //5
        "and    r0,%1\n\t"
        "breq   5f\n\t"
        "mov    r0,r21\n\t"
        "5:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //6
        "and    r0,%1\n\t"
        "breq   6f\n\t"
        "mov    r0,r21\n\t"
        "6:\n\t"
        "out    0x08,r0\n\t"

        "ld     r0,x+\n\t"      //7     -2
        "and    r0,%1\n\t"
        "breq   7f\n\t"
        "mov    r0,r21\n\t"
        "7:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //8
        "and    r0,%1\n\t"
        "breq   8f\n\t"
        "mov    r0,r21\n\t"
        "8:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //9
        "and    r0,%1\n\t"
        "breq   9f\n\t"
        "mov    r0,r21\n\t"
        "9:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //10
        "and    r0,%1\n\t"
        "breq   10f\n\t"
        "mov    r0,r21\n\t"
        "10:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //11
        "and    r0,%1\n\t"
        "breq   11f\n\t"
        "mov    r0,r21\n\t"
        "11:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //12
        "and    r0,%1\n\t"
        "breq   12f\n\t"
        "mov    r0,r21\n\t"
        "12:\n\t"
        "out    0x08,r0\n\t"

         "ld     r0,x+\n\t"      //13   -3
        "and    r0,%1\n\t"
        "breq   13f\n\t"
        "mov    r0,r21\n\t"
        "13:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //14
        "and    r0,%1\n\t"
        "breq   14f\n\t"
        "mov    r0,r21\n\t"
        "14:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //15
        "and    r0,%1\n\t"
        "breq   15f\n\t"
        "mov    r0,r21\n\t"
        "15:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //16
        "and    r0,%1\n\t"
        "breq   16f\n\t"
        "mov    r0,r21\n\t"
        "16:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //17
        "and    r0,%1\n\t"
        "breq   17f\n\t"
        "mov    r0,r21\n\t"
        "17:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //18
        "and    r0,%1\n\t"
        "breq   18f\n\t"
        "mov    r0,r21\n\t"
        "18:\n\t"
        "out    0x08,r0\n\t"

        "ld     r0,x+\n\t"      //19    -4
        "and    r0,%1\n\t"
        "breq   19f\n\t"
        "mov    r0,r21\n\t"
        "19:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //20
        "and    r0,%1\n\t"
        "breq   20f\n\t"
        "mov    r0,r21\n\t"
        "20:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //21
        "and    r0,%1\n\t"
        "breq   21f\n\t"
        "mov    r0,r21\n\t"
        "21:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //22
        "and    r0,%1\n\t"
        "breq   22f\n\t"
        "mov    r0,r21\n\t"
        "22:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //23
        "and    r0,%1\n\t"
        "breq   23f\n\t"
        "mov    r0,r21\n\t"
        "23:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //24
        "and    r0,%1\n\t"
        "breq   24f\n\t"
        "mov    r0,r21\n\t"
        "24:\n\t"
        "out    0x08,r0\n\t"

        "ld     r0,x+\n\t"      //25    -5
        "and    r0,%1\n\t"
        "breq   25f\n\t"
        "mov    r0,r21\n\t"
        "25:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //26
        "and    r0,%1\n\t"
        "breq   26f\n\t"
        "mov    r0,r21\n\t"
        "26:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //27
        "and    r0,%1\n\t"
        "breq   27f\n\t"
        "mov    r0,r21\n\t"
        "27:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //28
        "and    r0,%1\n\t"
        "breq   28f\n\t"
        "mov    r0,r21\n\t"
        "28:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //29
        "and    r0,%1\n\t"
        "breq   29f\n\t"
        "mov    r0,r21\n\t"
        "29:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //30
        "and    r0,%1\n\t"
        "breq   30f\n\t"
        "mov    r0,r21\n\t"
        "30:\n\t"
        "out    0x08,r0\n\t"

        "ld     r0,x+\n\t"      //31    -6
        "and    r0,%1\n\t"
        "breq   31f\n\t"
        "mov    r0,r21\n\t"
        "31:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //32
        "and    r0,%1\n\t"
        "breq   32f\n\t"
        "mov    r0,r21\n\t"
        "32:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //33
        "and    r0,%1\n\t"
        "breq   33f\n\t"
        "mov    r0,r21\n\t"
        "33:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //34
        "and    r0,%1\n\t"
        "breq   34f\n\t"
        "mov    r0,r21\n\t"
        "34:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //35
        "and    r0,%1\n\t"
        "breq   35f\n\t"
        "mov    r0,r21\n\t"
        "35:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //36
        "and    r0,%1\n\t"
        "breq   36f\n\t"
        "mov    r0,r21\n\t"
        "36:\n\t"
        "out    0x08,r0\n\t"

        "ld     r0,x+\n\t"      //37    -7
        "and    r0,%1\n\t"
        "breq   37f\n\t"
        "mov    r0,r21\n\t"
        "37:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //38
        "and    r0,%1\n\t"
        "breq   38f\n\t"
        "mov    r0,r21\n\t"
        "38:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //39
        "and    r0,%1\n\t"
        "breq   39f\n\t"
        "mov    r0,r21\n\t"
        "39:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //40
        "and    r0,%1\n\t"
        "breq   40f\n\t"
        "mov    r0,r21\n\t"
        "40:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //41
        "and    r0,%1\n\t"
        "breq   41f\n\t"
        "mov    r0,r21\n\t"
        "41:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //42
        "and    r0,%1\n\t"
        "breq   42f\n\t"
        "mov    r0,r21\n\t"
        "42:\n\t"
        "out    0x08,r0\n\t"

        "ld     r0,x+\n\t"      //43    -8
        "and    r0,%1\n\t"
        "breq   43f\n\t"
        "mov    r0,r21\n\t"
        "43:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //44
        "and    r0,%1\n\t"
        "breq   44f\n\t"
        "mov    r0,r21\n\t"
        "44:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //45
        "and    r0,%1\n\t"
        "breq   45f\n\t"
        "mov    r0,r21\n\t"
        "45:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //46
        "and    r0,%1\n\t"
        "breq   46f\n\t"
        "mov    r0,r21\n\t"
        "46:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //47
        "and    r0,%1\n\t"
        "breq   47f\n\t"
        "mov    r0,r21\n\t"
        "47:\n\t"
        "out    0x08,r0\n\t"
        "ld     r0,x+\n\t"      //48
        "and    r0,%1\n\t"
        "breq   48f\n\t"
        "mov    r0,r21\n\t"
        "48:\n\t"
        "out    0x08,r0\n\t"

        :
        : "x"(AddrGRP) ,"r"(MaskData) ,"r"(Color)
        :

    );
}

void CINT::OutData_64(uint16_t UH_AddrGRP )
{
    __asm__ __volatile("\n\t"
        "lpm r0,z+\n\t"     //1 Load Program Memory
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //2
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //3
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //4
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //5
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //6
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //7
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //8
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //9
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //10
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //11
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //12
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //13
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //14
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //15
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //16
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //17
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //18
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //19
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //20
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //21
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //22
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //23
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //24
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //25
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //26
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //27
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //28
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //29
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //30
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //31
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //32
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //33
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //34
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //35
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //36
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //37
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //38
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //39
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //40
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //41
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //42
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //43
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //44
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //45
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //46
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //47
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //48
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //49
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //50
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //51
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //52
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //53
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //54
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //55
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //56
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //57
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //58
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //59
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //60
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //61
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //62
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //63
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //64
        "out 0x08,r0\n\t"

        :
        : "z"(UH_AddrGRP)
        :
    );
}

void CINT::OutData_70(uint16_t UH_AddrGRP )
{
    __asm__ __volatile("\n\t"
        "lpm r0,z+\n\t"     //1 Load Program Memory
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //2
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //3
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //4
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //5
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //6
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //7
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //8
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //9
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //10
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //11
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //12
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //13
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //14
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //15
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //16
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //17
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //18
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //19
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //20
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //21
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //22
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //23
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //24
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //25
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //26
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //27
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //28
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //29
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //30
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //31
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //32
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //33
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //34
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //35
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //36
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //37
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //38
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //39
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //40
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //41
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //42
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //43
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //44
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //45
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //46
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //47
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //48
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //49
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //50
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //51
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //52
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //53
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //54
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //55
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //56
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //57
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //58
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //59
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //60
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //61
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //62
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //63
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //64
        "out 0x08,r0\n\t"

        "lpm r0,z+\n\t"     //65
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //66
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //67
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //68
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //69
        "out 0x08,r0\n\t"
        "lpm r0,z+\n\t"     //70
        "out 0x08,r0\n\t"


        :
        : "z"(UH_AddrGRP)
        :
    );
}










