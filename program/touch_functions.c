/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm324xg_lcd_sklin.h"
#include "touch_module.h"
//#include "user_defined.h"
//#include <stdio.h>	// for sprintf

void Driver_GPIO(void);
void Default_Calibration(void);

//===============================
/* Initialize the Touch module */
void Touch_Init(void)
{
	Default_Calibration();
	Driver_GPIO();
}

//===============================
void Touch_TextBox_Draw(uint16_t textColor, uint16_t backColor,
									uint8_t Line, uint16_t Col, uint8_t BlankLines, char *ptr)
{
	uint16_t Tcolor, Bcolor;
	uint8_t i;

	LCD_GetColors(&Tcolor, &Bcolor);
	LCD_SetColors(textColor, backColor); 
	for (i=0; i<(BlankLines/2); i++)
	{
		LCD_DisplayStringLineCol(Line-1, Col, "   ");
		LCD_DisplayStringLineCol(Line+1, Col, "   ");
	}
	LCD_DisplayStringLineCol(Line, Col, ptr);
	LCD_SetColors(Tcolor, Bcolor); 
}

//===============================
uint8_t Check_TextBox_touch(uint8_t Line, uint16_t Col, uint8_t NrChar, uint8_t BlankLines)
{
	sFONT *pFont;
	uint16_t Xpos1, Ypos1, Xpos2, Ypos2;
	TS_StateTypeDef  TS_State;
	uint16_t x, y;
	int8_t i;
	
		TS_GetState(&TS_State);
	
		if(TS_State.TouchDetected){
			x = TS_State.x;
			y = TS_State.y;

			pFont = LCD_GetFont();
			Xpos1 = pFont->Width* Col;
			Xpos2 = pFont->Width* (Col+NrChar);
			i = Line - BlankLines/2;
			if (i < 0) i = 0;
			Ypos1 = pFont->Height* i;
			Ypos2 = pFont->Height* (Line+1+BlankLines/2);
			
			if( (x >= Xpos1) &&	 (x < Xpos2 )	)
			{		
					if((y >= Ypos1)&& (y < Ypos2 ))					
					{
						return 1;
					}
			}
		}
		return 0;
}

