
int num;

void display(void) {

	switch(num){
	case 0:	
		LCD_DisplayStringAt(3, 3, "a. Background", Middle_MODE);
		LCD_DrawRGBImage( 10, 20, 16, 16, (uint8_t*)0x08010800);
		LCD_DisplayStringAt(35, 20, "This scene takes place in the desert.", LEFT_MODE);

		LCD_DisplayStringAt(3, 50, "b. Character", Middle_MODE);	
		LCD_DrawRGBImage( 140, 70, 16, 16, (uint8_t*)0x08010600);
		LCD_DrawRGBImage( 180, 70, 16, 16, (uint8_t*)0x08010a00);
		LCD_DisplayStringAt(10, 95, "The player controls a chameleon. The left image shows the chameleon in its normal state; the right image shows it in stealth mode.", LEFT_MODE);

		LCD_DisplayStringAt(3, , "c. Enemies", Middle_MODE);
		LCD_DrawRGBImage( 10, 20, 16, 16, (uint8_t*)0x08011800);
		LCD_DisplayStringAt(35, 20, "Rabid Wild Dog: Extremely fast, can move two tiles per turn", LEFT_MODE);
		LCD_DrawRGBImage( 10, 70, 16, 16, (uint8_t*)0x08011a00);
		LCD_DisplayStringAt(35, 70, "Desert Devourer Beast: Moves at the same speed as the chameleon.", LEFT_MODE);
		LCD_DisplayStringAt(10, 120, "P.S.: Unlike the fragile chameleon, desert enemies are incredibly resilient. Special items may only knock them down temporarily—they're so tough that not even a bomb can destroy those starving monsters.", LEFT_MODE);
		break;
    case 1:
		LCD_DisplayStringAt(3, 3, "d. Items", Middle_MODE);	
	case 3:
	
    }
    

}
