#include "game.h"
extern void delay_ms(uint32_t wait_ms);

void draw_hit(uint16_t x, uint16_t y) {
	LCD_SetTextColor(RED);
	uint16_t y_end = y + (15);
	uint16_t spacing = (15) / (4);

	for (int i = 1; i <= 3; i++) {
		uint16_t offset = i * spacing;          
		uint16_t x0 = x;                         
		uint16_t y0 = y_end - offset;            
		uint16_t x1 = x + offset;                 
		uint16_t y1 = y_end;                      
		LCD_DrawLine(x0, y0, x1, y1);
	}
}

#define BITE_OFFSET_Y 2  
void draw_bite(uint16_t x, uint16_t y) {
	LCD_SetTextColor(WHITE);
	static const uint8_t vx_up[7] = { 2,  4,  6,  8, 10, 12, 14};
	static const uint8_t vy_up[7] = { 4,  2,  4,  2,  4,  2,  4};
	static const uint8_t vx_down[5] = { 5,   7,  9,  11,  9 };
	static const uint8_t vy_down[5] = {10,  12, 10,  12, 10};

	for (int i = 0; i < 6; i++) {
		uint16_t x0 = x + vx_up[i];
		uint16_t y0 = y + vy_up[i] + 2;
		uint16_t x1 = x + vx_up[i+1];
		uint16_t y1 = y + vy_up[i+1] + 2;
		LCD_DrawLine(x0, y0, x1, y1);
	}
	for (int i = 0; i < 4; i++) {
		uint16_t x0 = x + vx_down[i];
		uint16_t y0 = y + vy_down[i]+ 2;
		uint16_t x1 = x + vx_down[i+1];
		uint16_t y1 = y + vy_down[i+1] + 2;
		LCD_DrawLine(x0, y0, x1, y1);
	}
}

void draw_slash(uint16_t x, uint16_t y) {
	LCD_SetTextColor(DARKMAGENTA);
	LCD_DrawLine(x, y, x + 15, y + 15);
	LCD_DrawLine(x, y + 15, x + 15, y);
}

void draw_splatter(uint16_t x, uint16_t y) {
	LCD_SetTextColor(DARKMAGENTA);
	uint16_t cx = x + 7;
	uint16_t cy = y + 7;
	LCD_FillRect(cx - 2, cy - 2, 5, 5);  
	for (int d = 0; d < 4; d++) {
		int dx = (d == 1) - (d == 3);    
		int dy = (d == 2) - (d == 0); 
		uint16_t px = cx, py = cy;
		for (int s = 0; s < 4; s++) {
			px += dx;
			py += dy;
			LCD_DrawPixel(px, py, DARKMAGENTA);
		}
	}
	for (int d = 0; d < 4; d++) {
		int dx = (d < 2) ?  1 : -1;    
		int dy = (d == 0 || d == 3) ? -1 : 1; 
		uint16_t px = cx, py = cy;
		for (int s = 0; s < 3; s++) {
			px += dx;
			py += dy;
			LCD_DrawPixel(px, py, DARKMAGENTA);
		}
	}
}

void draw_border(uint16_t x, uint16_t y) {        
	for (int i = 0; i < 16; i++) {
		LCD_DrawPixel(x + i, y, RED);            // top
		LCD_DrawPixel(x + i, y + 16-1, RED);     // bottom
	}
	for (int i = 1; i < 16-1; i++) {
		LCD_DrawPixel(x, y + i, GREEN);          // left
		LCD_DrawPixel(x + 16-1, y + i, GREEN);   // right
	}
}

void draw_heal(uint16_t x, uint16_t y) {
	LCD_SetTextColor(LCD_COLOR_RED);
	static const uint8_t heart[8][8] = {
		{0,1,1,0,  0,1,1,0},
		{1,1,1,1,  1,1,1,1},
		{1,1,1,1,  1,1,1,1},
		{0,1,1,1,  1,1,1,0},
		{0,0,1,1,  1,1,0,0},
		{0,0,0,1,  1,0,0,0},
		{0,0,0,0,  1,0,0,0},
		{0,0,0,0,  0,0,0,0}
	};
	uint16_t ox = x + 4;
	uint16_t oy = y + 4;
	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			if (heart[r][c]) {
				LCD_DrawPixel(ox + c, oy + r, LCD_COLOR_RED);
			}
		}
	}
}

void draw_boom(uint16_t x, uint16_t y) {
	LCD_SetTextColor(RED);
	uint16_t cx = x + 7;
	uint16_t cy = y + 7;
	LCD_FillRect(cx - 2, cy - 2, 5, 5);  
	for (int d = 0; d < 4; d++) {
		int dx = (d == 1) - (d == 3);    
		int dy = (d == 2) - (d == 0); 
		uint16_t px = cx, py = cy;
		for (int s = 0; s < 4; s++) {
			px += dx;
			py += dy;
			LCD_DrawPixel(px, py, RED);
		}
	}
	for (int d = 0; d < 4; d++) {
		int dx = (d < 2) ?  1 : -1;    
		int dy = (d == 0 || d == 3) ? -1 : 1; 
		uint16_t px = cx, py = cy;
		for (int s = 0; s < 3; s++) {
			px += dx;
			py += dy;
			LCD_DrawPixel(px, py, RED);
		}
	}
}

TS_StateTypeDef  Touch;    //control touch 
int touch_x, touch_y;
int all_page = 6;
int all_level = 6;
static Point btnUp, btnDown, btnLeft, btnRight;
uint8_t tag = 0;
uint8_t portal_tag = 0;
uint16_t occupy[15][15];      
char S1[10];
uint16_t tem_x,tem_y;
uint8_t time_stop;
uint8_t random;
uint8_t click_num = 0;

struct character{
	uint16_t X ;
	uint16_t Y ;
	uint16_t pre_X ;
	uint16_t pre_Y ;
	uint8_t* image;  
	uint8_t* hide_image;
	int life;  
	uint8_t skill;
	uint8_t skill_round;
	uint8_t* skill_image;
	uint8_t* unskill_image;
} player;

struct enemy{
	uint16_t X ;
	uint16_t Y ;
	uint16_t pre_X ;
	uint16_t pre_Y ;
	uint8_t* image;    // one image + 200
} enemy1, enemy2, enemy3, enemy4, enemy5, enemy6, enemy7, enemy8, enemy9, enemy10;

struct portal{
	uint16_t X ;  
	uint16_t Y ;  
} portal_1,portal_2,portal_3,portal_4,portal_5,portal_6,portal_7,portal_8,portal_9,portal_10
 ,portal_11,portal_12,portal_13,portal_14,portal_15,portal_16,portal_17,portal_18,portal_19,portal_20
 ,portal_21,portal_22,portal_23,portal_24,portal_25,portal_26,portal_27,portal_28,portal_29,portal_30;

// #-------------------------#
//        state == 0
// #-------------------------#
void start_screen(void) {
	click_num = 0;
	LCD_DrawRGBImage( 0, 0, 320, 240, (uint8_t*)0x08040000);
	LCD_SetTextColor(MAGENTA);
	LCD_SetFont(&Font16);
	LCD_DisplayStringAt(0, 160, "Start Game", CENTER_MODE);	
	LCD_DisplayStringAt(0, 180, "  Levels  ", CENTER_MODE);
	LCD_DisplayStringAt(0, 200, " Overview ", CENTER_MODE);
}

uint8_t control_screen(void) {
	TSC_TouchDet(1);  
	TS_GetState(&Touch);
	touch_x = -1;
	touch_y = -1;
	if (Touch.TouchDetected){
		touch_x = Touch.x;
		touch_y = Touch.y;
		WaitForTouchRelease(6);
	}
	if(touch_x >= 130 && touch_x <= 180 && touch_y >= 93 && touch_y <= 127 && click_num >= 9)
		return 8;
	else if(touch_x >= 130 && touch_x <= 180 && touch_y >= 93 && touch_y <= 127)
		click_num++;
	else if(touch_x >= 120 && touch_x <= 200 && touch_y >= 160 && touch_y <= 177)
			return 2;
	else if(touch_x >= 120 && touch_x <= 200 && touch_y >= 180 && touch_y <= 197)
			return 17;
	else if(touch_x >= 120 && touch_x <= 200 && touch_y >= 200 && touch_y <= 217)
			return 1;
	return 0;
}

// #-------------------------#
//        state == 1
// #-------------------------#
void overview(page){
	switch(page){
		case 0:	
			LCD_SetTextColor(BLACK);
			LCD_FillRect(0, 0, 320, 240);
			LCD_SetTextColor(RED);
			LCD_FillRect(290, 202, 30, 38);
			LCD_SetFont(&Font20);		
			LCD_DisplayStringAt(292, 204, "EX",LEFT_MODE);
			LCD_DisplayStringAt(292, 222, "IT",LEFT_MODE);
	
			LCD_SetTextColor(DARKGREEN);
			LCD_SetFont(&Font16);				
			LCD_DisplayStringAt(0, 3, "a. Background", CENTER_MODE);
			LCD_DrawRGBImage( 10, 20, 16, 16, (uint8_t*)0x08010800);
			LCD_SetTextColor(YELLOW);
			LCD_SetFont(&Font12);
			LCD_DisplayStringAt(30, 23, " => This scene takes place in the desert.", LEFT_MODE);
			
			LCD_SetTextColor(DARKGREEN);
			LCD_SetFont(&Font16);
			LCD_DisplayStringAt(0, 53, "b. Character", CENTER_MODE);	
			LCD_DrawRGBImage( 140, 70, 16, 16, (uint8_t*)0x08010600);
			LCD_DrawRGBImage( 170, 70, 16, 16, (uint8_t*)0x08010a00);
			LCD_SetTextColor(YELLOW);
			LCD_SetFont(&Font12);
			LCD_DisplayStringAt(0, 87, " => The player controls a chameleon:", LEFT_MODE);
			LCD_DisplayStringAt(0, 101, "    1. The chameleon keeps its color.", LEFT_MODE);
			LCD_DisplayStringAt(0, 115, "    2. The chameleon changes its color.", LEFT_MODE);
			LCD_DrawRGBImage( 90, 130, 64, 64, (uint8_t*)0x080A2800);
			LCD_DrawRGBImage( 166, 130, 64, 64, (uint8_t*)0x080a0800);
			LCD_DisplayStringAt(0, 195, " => Chameleon Skill Bar:", LEFT_MODE);
			LCD_DisplayStringAt(0, 209, "    1. The skill is Invisibility Ready.", LEFT_MODE);
			LCD_DisplayStringAt(0, 223, "    2. The skill is Invisibility Active.", LEFT_MODE);
			break;
				
		case 1:
			LCD_SetTextColor(BLACK);
			LCD_FillRect(0, 0, 320, 240);
			LCD_SetTextColor(RED);
			LCD_FillRect(290, 202, 30, 38);
			LCD_SetFont(&Font20);		
			LCD_DisplayStringAt(292, 204, "EX",LEFT_MODE);
			LCD_DisplayStringAt(292, 222, "IT",LEFT_MODE);
	
			LCD_SetTextColor(DARKGREEN);
			LCD_SetFont(&Font16);
			LCD_DisplayStringAt(0, 3, "c. Enemies", CENTER_MODE);	
			LCD_DrawRGBImage( 140, 23, 16, 16, (uint8_t*)0x08011800);
			LCD_DrawRGBImage( 170, 23, 16, 16, (uint8_t*)0x08011600);
			LCD_SetTextColor(YELLOW);
			LCD_SetFont(&Font12);
			LCD_DisplayStringAt(0, 43, " => 1. Wild Dog: 2 tiles/turn, -1 HP.", LEFT_MODE);
			LCD_DisplayStringAt(0, 60, "    2. Bigmouth Beast: 1 tile/turn, -1 HP.", LEFT_MODE);
			
			LCD_SetTextColor(DARKGREEN);
			LCD_SetFont(&Font16);	
			LCD_DisplayStringAt( 0, 90, "d. Items", CENTER_MODE);
			LCD_DrawRGBImage( 110, 110, 16, 16, (uint8_t*)0x08011a00);
			LCD_DrawRGBImage( 140, 110, 16, 16, (uint8_t*)0x08011e00);
			LCD_DrawRGBImage( 170, 110, 16, 16, (uint8_t*)0x08011c00);
			LCD_DrawRGBImage( 200, 110, 16, 16, (uint8_t*)0x08012000);
			LCD_SetTextColor(YELLOW);
			LCD_SetFont(&Font12);
			LCD_DisplayStringAt(0, 130, " => 1. Bomb: Instant death on contact.", LEFT_MODE);
			LCD_DisplayStringAt(0, 147, "    2. Tornado: Blow enemies to corner edges.", LEFT_MODE);		
			LCD_DisplayStringAt(0, 164, "    3. Potion: +3 HP.", LEFT_MODE);
			LCD_DisplayStringAt(0, 181, "    4. Goal: Only way escape.", LEFT_MODE);
			LCD_SetTextColor(LIGHTMAGENTA);						
			LCD_DisplayStringAt(0, 206, " Note: Enemies destroy items on contact.", LEFT_MODE);	
			break;

		case 2:	
			LCD_SetTextColor(BLACK);
			LCD_FillRect(0, 0, 320, 240);
			LCD_SetTextColor(RED);
			LCD_FillRect(290, 202, 30, 38);
			LCD_SetFont(&Font20);		
			LCD_DisplayStringAt(292, 204, "EX",LEFT_MODE);
			LCD_DisplayStringAt(292, 222, "IT",LEFT_MODE);
	
			LCD_SetTextColor(DARKGREEN);
			LCD_SetFont(&Font16);				
			LCD_DisplayStringAt(0, 3, "a. Background", CENTER_MODE);
			LCD_DrawRGBImage( 10, 20, 16, 16, (uint8_t*)0x08010e00);
			LCD_SetTextColor(BLUE);
			LCD_SetFont(&Font12);
			LCD_DisplayStringAt(30, 23, " => This is underwater scene.", LEFT_MODE);
			
			LCD_SetTextColor(DARKGREEN);
			LCD_SetFont(&Font16);
			LCD_DisplayStringAt(0, 53, "b. Character", CENTER_MODE);	
			LCD_DrawRGBImage( 140, 70, 16, 16, (uint8_t*)0x08010c00);
			LCD_DrawRGBImage( 170, 70, 16, 16, (uint8_t*)0x08011000);
			LCD_SetTextColor(BLUE);
			LCD_SetFont(&Font12);
			LCD_DisplayStringAt(0, 87, " => The player controls a chameleon:", LEFT_MODE);
			LCD_DisplayStringAt(0, 101, "    1. The chameleon keeps its color.", LEFT_MODE);
			LCD_DisplayStringAt(0, 115, "    2. The chameleon changes its color.", LEFT_MODE);
			LCD_DrawRGBImage( 90, 130, 64, 64, (uint8_t*)0x080A2800);
			LCD_DrawRGBImage( 166, 130, 64, 64, (uint8_t*)0x080a0800);
			LCD_DisplayStringAt(0, 195, " => Chameleon Skill Bar:", LEFT_MODE);
			LCD_DisplayStringAt(0, 209, "    1. The skill is Invisibility Ready.", LEFT_MODE);
			LCD_DisplayStringAt(0, 223, "    2. The skill is Invisibility Active.", LEFT_MODE);
			break;
				
		case 3:
			LCD_SetTextColor(BLACK);
			LCD_FillRect(0, 0, 320, 240);
			LCD_SetTextColor(RED);
			LCD_FillRect(290, 202, 30, 38);
			LCD_SetFont(&Font20);		
			LCD_DisplayStringAt(292, 204, "EX",LEFT_MODE);
			LCD_DisplayStringAt(292, 222, "IT",LEFT_MODE);
	
			LCD_SetTextColor(DARKGREEN);
			LCD_SetFont(&Font16);
			LCD_DisplayStringAt(0, 3, "c. Enemies", CENTER_MODE);	
			LCD_DrawRGBImage( 122, 23, 16, 16, (uint8_t*)0x08012800);
			LCD_DrawRGBImage( 152, 23, 16, 16, (uint8_t*)0x08012a00);
			LCD_DrawRGBImage( 182, 23, 16, 16, (uint8_t*)0x08012c00);
			LCD_SetTextColor(BLUE);
			LCD_SetFont(&Font12);
			LCD_DisplayStringAt(0, 43, "=>  1. Giant Grouper: 2 tiles/turn, -1 HP.", LEFT_MODE);
			LCD_DisplayStringAt(0, 60, "    2. Anglerfish: 2 tiles/turn, -2 HP.", LEFT_MODE);
			LCD_DisplayStringAt(0, 77, "    3. Greedy Fish: 1 tile/turn, -1 HP,", LEFT_MODE);
			LCD_DisplayStringAt(0, 94, "       drops feces while moving.", LEFT_MODE);
			
			LCD_SetTextColor(DARKGREEN);
			LCD_SetFont(&Font16);	
			LCD_DisplayStringAt( 0, 114, "d. Items", CENTER_MODE);
			LCD_DrawRGBImage( 110, 134, 16, 16, (uint8_t*)0x08013000);
			LCD_DrawRGBImage( 140, 134, 16, 16, (uint8_t*)0x08012e00);
			LCD_DrawRGBImage( 170, 134, 16, 16, (uint8_t*)0x08013200);
			LCD_DrawRGBImage( 200, 134, 16, 16, (uint8_t*)0x08012200);
			LCD_SetTextColor(BLUE);
			LCD_SetFont(&Font12);
			LCD_DisplayStringAt(0, 154, "=>  1. Potion: +3 HP.", LEFT_MODE);
			LCD_DisplayStringAt(0, 171, "    2. Feces: -1 HP on contact.", LEFT_MODE);		
			LCD_DisplayStringAt(0, 188, "    3. Portal: Teleports to a unknown portal.", LEFT_MODE);
			LCD_DisplayStringAt(0, 205, "    4. Goal: Only way escape.", LEFT_MODE);
			LCD_SetTextColor(LIGHTMAGENTA);						
			LCD_DisplayStringAt(0, 228, " Note: Portals don't break & enemy-proof.", LEFT_MODE);								
			break;
				
		case 4:	
			LCD_SetTextColor(BLACK);
			LCD_FillRect(0, 0, 320, 240);
			LCD_SetTextColor(DARKYELLOW);
			LCD_FillRect(290, 202, 30, 38);
			LCD_SetFont(&Font20);		
			LCD_DisplayStringAt(292, 204, "EX",LEFT_MODE);
			LCD_DisplayStringAt(292, 222, "IT",LEFT_MODE);
	
			LCD_SetTextColor(DARKGREEN);
			LCD_SetFont(&Font16);				
			LCD_DisplayStringAt(0, 3, "a. Background", CENTER_MODE);
			LCD_DrawRGBImage( 10, 20, 16, 16, (uint8_t*)0x08014000);
			LCD_SetTextColor(RED);
			LCD_SetFont(&Font12);
			LCD_DisplayStringAt(30, 23, " => This is Volcano zone.", LEFT_MODE);
			
			LCD_SetTextColor(DARKGREEN);
			LCD_SetFont(&Font16);
			LCD_DisplayStringAt(0, 53, "b. Character", CENTER_MODE);	
			LCD_DrawRGBImage( 140, 70, 16, 16, (uint8_t*)0x08014200);
			LCD_DrawRGBImage( 170, 70, 16, 16, (uint8_t*)0x08014400);
			LCD_SetTextColor(RED);
			LCD_SetFont(&Font12);
			LCD_DisplayStringAt(0, 87, " => The player controls a chameleon:", LEFT_MODE);
			LCD_DisplayStringAt(0, 101, "    1. The chameleon keeps its color.", LEFT_MODE);
			LCD_DisplayStringAt(0, 115, "    2. The chameleon changes its color.", LEFT_MODE);
			LCD_DrawRGBImage( 90, 130, 64, 64, (uint8_t*)0x080A2800);
			LCD_DrawRGBImage( 166, 130, 64, 64, (uint8_t*)0x080a0800);
			LCD_DisplayStringAt(0, 195, " => Chameleon Skill Bar:", LEFT_MODE);
			LCD_DisplayStringAt(0, 209, "    1. The skill is Invisibility Ready.", LEFT_MODE);
			LCD_DisplayStringAt(0, 223, "    2. The skill is Invisibility Active.", LEFT_MODE);
			break;
				
		case 5:
			LCD_SetTextColor(BLACK);
			LCD_FillRect(0, 0, 320, 240);
			LCD_SetTextColor(DARKYELLOW);
			LCD_FillRect(290, 202, 30, 38);
			LCD_SetFont(&Font20);		
			LCD_DisplayStringAt(292, 204, "EX",LEFT_MODE);
			LCD_DisplayStringAt(292, 222, "IT",LEFT_MODE);
	
			LCD_SetTextColor(DARKGREEN);
			LCD_SetFont(&Font16);
			LCD_DisplayStringAt(0, 3, "c. Enemies", CENTER_MODE);	
			LCD_DrawRGBImage( 122, 23, 16, 16, (uint8_t*)0x08013400);
			LCD_DrawRGBImage( 152, 23, 16, 16, (uint8_t*)0x08013600);
			LCD_DrawRGBImage( 182, 23, 16, 16, (uint8_t*)0x08013800);
			LCD_SetTextColor(RED);
			LCD_SetFont(&Font12);
			LCD_DisplayStringAt(0, 43, "=>  1. Ghost: 2 tiles/turn, -1 HP,", LEFT_MODE);
			LCD_DisplayStringAt(0, 60, "       turns invisible randomly.", LEFT_MODE);	 
			LCD_DisplayStringAt(0, 77, "    2. Volcano Wolf: 3 tiles/turn, -1 HP.", LEFT_MODE);
			LCD_DisplayStringAt(0, 94, "    3. Magma Giant: 1 tile/turn, touching", LEFT_MODE);				
			LCD_DisplayStringAt(0, 111,"       up/down/left/right = instant death.", LEFT_MODE);
			
			LCD_SetTextColor(DARKGREEN);
			LCD_SetFont(&Font16);	
			LCD_DisplayStringAt( 0, 131, "d. Items", CENTER_MODE);
			LCD_DrawRGBImage( 122, 151, 16, 16, (uint8_t*)0x08013a00);
			LCD_DrawRGBImage( 152, 151, 16, 16, (uint8_t*)0x08013c00);
			LCD_DrawRGBImage( 182, 151, 16, 16, (uint8_t*)0x08013e00);
			LCD_SetTextColor(RED);
			LCD_SetFont(&Font12);
			LCD_DisplayStringAt(0, 171, "=>  1. Rock: Just impassable.", LEFT_MODE);
			LCD_DisplayStringAt(0, 188, "    2. time Stopper: Freeze enemies 2 turns.", LEFT_MODE);		
			LCD_DisplayStringAt(0, 205, "    3. Goal: Only way escape.", LEFT_MODE);
			LCD_SetTextColor(LIGHTMAGENTA);						
			LCD_DisplayStringAt(0, 228, " Tip: Block enemies with rocks to win.", LEFT_MODE);						
			break;
	 }
}

int control_page(page) {
	TSC_TouchDet(1);  
	TS_GetState(&Touch);
	touch_x = -1;
	touch_y = -1;
	if (Touch.TouchDetected){
		touch_x = Touch.x;
		touch_y = Touch.y;
		WaitForTouchRelease(6);
	}
	if(touch_x >= 290 && touch_x <= 320 && touch_y >= 202 && touch_y <= 240)
		return 1000;
	else if(touch_x >= 160 && touch_x <= 320 && touch_y >= 0 && touch_y <= 240){
	 if(page == all_page - 1)
		 return 0;
	 else 
		 return page + 1;
	}
	else if(touch_x >= 0 && touch_x <= 160 && touch_y >= 0 && touch_y <= 240){
	 if(page == 0)
		 return all_page - 1;
	 else 
		 return page - 1;
	}
	return page;
}

// #-------------------------#
//        state == 2
// #-------------------------#
// 1: enemy, 2: boom, 3:tornado, 4: blood, 5:end
void game_init(void) {
	LCD_SetTextColor(BLACK);
	LCD_FillRect(0, 0, 320, 240);
	for(int i=0;i<=14;i++)
		 for(int j=0;j<=14;j++)
				occupy[i][j] = 0;

	player.life = 3;
	player.X = 0;
	player.Y = 224;
	player.pre_X = player.X;
	player.pre_Y = player.Y;	
	player.skill = 2;            //0: noskill, 1: using, 2:has skill
	player.skill_round = 4;
	player.skill_image = (uint8_t*)0x080a0800;
	player.unskill_image = (uint8_t*)0x080a2800;
	LCD_DrawRGBImage( 245, 85, 64, 64, player.unskill_image);

	LCD_DrawRGBImage( 245, 30, 32, 32, (uint8_t*)0x080a0000);
	LCD_SetFont(&Font16);
	LCD_SetTextColor(RED);
	LCD_DisplayStringAt(280, 40, "X", LEFT_MODE);
	sprintf(S1, "%d", player.life);
	LCD_DisplayStringAt(295, 40, S1, LEFT_MODE);
	
	btnUp = (Point){ 245 + 27, 170};
	btnDown = (Point){ 245 + 27, 212 };
	btnLeft = (Point){ 245 , 195 };
	btnRight = (Point){ 245 + 43, 195 };
	LCD_SetTextColor(WHITE);
	LCD_FillRect(245 + 27, 170, 15, 25);   //up
	LCD_FillRect(245 + 27, 212,  15, 25);  //down
	LCD_FillRect(245 , 195 , 25, 15);      //left
	LCD_FillRect(245 + 43, 195, 25, 15);   //right
}

void level_1_draw(void) {	  
	for(int i = 0; i <= 14 ; i++){
		for(int j = 0; j <= 14 ;j++){
			LCD_DrawRGBImage(i*16, j*16, 16, 16, (uint8_t*)0x08010800); 
		}
	}
	
	player.image = (uint8_t*)0x08010600;		
	player.hide_image = (uint8_t*)0x08010a00;
	LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
	
	occupy[14][0] = 5;
	LCD_DrawRGBImage(224, 0, 16, 16, (uint8_t*)0x08012000); 
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy1.image = (uint8_t*)0x08011600;
	enemy1.X = tem_x * 16;
	enemy1.Y = tem_y * 16;
	enemy1.pre_X = enemy1.X;
	enemy1.pre_Y = enemy1.Y;
	LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, enemy1.image);  
		
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy2.image = (uint8_t*)0x08011800;
	enemy2.X = tem_x * 16;
	enemy2.Y = tem_y * 16;
	enemy2.pre_X = enemy2.X;
	enemy2.pre_Y = enemy2.Y;
	LCD_DrawRGBImage(enemy2.X, enemy2.Y, 16, 16, enemy2.image);  

	for(int i = 0; i <= 4; i++){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);
		while(occupy[tem_x][tem_y] != 0){
			tem_x = RNG_Get_RandomRange(1,13);
			tem_y = RNG_Get_RandomRange(1,13);		    
		}		
		occupy[tem_x][tem_y] = 2;
		LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08011a00);
	}
	
	for(int i = 0; i <= 2; i++){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);
		while(occupy[tem_x][tem_y] != 0){
			tem_x = RNG_Get_RandomRange(1,13);
			tem_y = RNG_Get_RandomRange(1,13);		    
		}		
		occupy[tem_x][tem_y] = 3;
		LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08011e00);
	}
	
	for(int i = 0; i <= 4; i++){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);
		while(occupy[tem_x][tem_y] != 0){
			tem_x = RNG_Get_RandomRange(1,13);
			tem_y = RNG_Get_RandomRange(1,13) ;		    
		}		
		occupy[tem_x][tem_y] = 4;
		LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08011c00);
	}
}

int level_1_flow(void) {
	tag = 0;
	TSC_TouchDet(1);  
	TS_GetState(&Touch);
	touch_x = -1;
	touch_y = -1;
	if (Touch.TouchDetected){
		touch_x = Touch.x;
		touch_y = Touch.y;
		WaitForTouchRelease(6);
	}

 if(touch_x >= 245 && touch_x <= 309 && touch_y >= 85 && touch_y <= 149 && player.skill == 2){
	 LCD_DrawRGBImage( 245, 85, 64, 64, player.skill_image);
	 player.skill = 1;
	 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);  
 }
 else if(touch_x >= btnUp.X && touch_x <= btnUp.X + 15 && touch_y >= btnUp.Y && touch_y <= btnUp.Y + 25 && player.Y != 0 && occupy[player.X/16][(player.Y-16)/16]!=1){  // up
		player.Y -= 16;
		tag = 1;
	}
 else if(touch_x >= btnDown.X && touch_x <= btnDown.X + 15 && touch_y >= btnDown.Y && touch_y <= btnDown.Y + 25 && player.Y != 224 && occupy[player.X/16][(player.Y+16)/16]!=1){ // down
		player.Y += 16;
		tag = 1;
 }
 else if(touch_x >= btnLeft.X && touch_x <= btnLeft.X + 25 && touch_y >= btnLeft.Y && touch_y <= btnLeft.Y + 15 && player.X != 0 && occupy[(player.X-16)/16][player.Y/16]!=1){ //left
		player.X -= 16;
		tag = 1;
 } 
 else if(touch_x >= btnRight.X && touch_x <= btnRight.X + 25 && touch_y >= btnRight.Y && touch_y <= btnRight.Y + 15 && player.X != 224 && occupy[(player.X+16)/16][player.Y/16]!=1){ // right
		player.X += 16;
		tag = 1;
 }
 
 if(tag){
	 if(player.skill == 0){
	 	 player.skill_round++;
		 if(player.skill_round == 4){
			 player.skill = 2;
			 LCD_DrawRGBImage( 245, 85, 64, 64, player.unskill_image);
		 }
	 }
	 else if(player.skill == 1){
		 player.skill_round--;
		 if(player.skill_round == 0){
			 player.skill = 0;
			 LCD_SetTextColor(BLACK);
			 LCD_FillRect(245, 85, 64, 64);
			 LCD_SetTextColor(WHITE);
		 }
	 }
	
	 if(player.skill != 1)
		 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);  
	 else
		 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);   				
	 LCD_DrawRGBImage(player.pre_X, player.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
	 player.pre_X = player.X;
	 player.pre_Y = player.Y;
 }
 
 if(tag && occupy[player.X/16][player.Y/16] > 1){
		if(occupy[player.X/16][player.Y/16] == 2){
			draw_boom(player.X, player.Y);
			delay_ms(1000);
			return 0;
		}
		else if(occupy[player.X/16][player.Y/16] == 3){
			tem_x = 0;
			tem_y = 0;
			while(occupy[tem_x][tem_y] != 0 || (player.X/16 == tem_x && player.Y/16 == tem_y))
			  tem_y++;
			enemy1.X = 0;
			enemy1.Y = tem_y * 16;
			LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, enemy1.image);   
			LCD_DrawRGBImage(enemy1.pre_X, enemy1.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
			occupy[enemy1.X/16][enemy1.Y/16] = 1;				
			occupy[enemy1.pre_X/16][enemy1.pre_Y/16] = 0;
			enemy1.pre_X = enemy1.X;
			enemy1.pre_Y = enemy1.Y; 
			
			tem_x = 0;
			tem_y = 14;					
			while(occupy[tem_x][tem_y] != 0 || (player.X/16 == tem_x && player.Y/16 == tem_y))
			  tem_x++;
			enemy2.X = tem_x * 16;
			enemy2.Y = 224;
			LCD_DrawRGBImage(enemy2.X, enemy2.Y, 16, 16, enemy2.image); 
			LCD_DrawRGBImage(enemy2.pre_X, enemy2.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
			occupy[enemy2.X/16][enemy2.Y/16] = 1;				
			occupy[enemy2.pre_X/16][enemy2.pre_Y/16] = 0;							
			enemy2.pre_X = enemy2.X;
			enemy2.pre_Y = enemy2.Y; 
			
			occupy[player.X/16][player.Y/16] = 0;
		}
		else if(occupy[player.X/16][player.Y/16] == 4){
			draw_heal(player.X, player.Y);
			delay_ms(500);
			if(player.skill == 1)
			  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			else
			  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);
			player.life += 3;
			occupy[player.X/16][player.Y/16] = 0;
		}
		else if(occupy[player.X/16][player.Y/16] == 5){
			LCD_SetTextColor(YELLOW);
			LCD_SetFont(&Font16);
			LCD_DisplayStringAt(0, 60, "!! Congratulation Success !!", CENTER_MODE);
			LCD_SetFont(&Font16);					
			LCD_DisplayStringAt(0, 130, "Click to next level", CENTER_MODE);
			LCD_DisplayStringAt(0, 160, "   Click to quit   ", CENTER_MODE);					
			return 1000;
		}
		
		LCD_SetTextColor(RED);
		sprintf(S1, "%d ", player.life);
		LCD_DisplayStringAt(295, 40, S1, LEFT_MODE);					
 }

 if(tag && player.skill != 1){
	 tag = 0;
		
	 if((player.X-16 == enemy1.X && player.Y == enemy1.Y) || (player.X+16 == enemy1.X && player.Y == enemy1.Y)
		 ||(player.Y-16 == enemy1.Y && player.X == enemy1.X) || (player.Y+16 == enemy1.Y && player.X == enemy1.X)){
		  player.life -= 1; 	
		  draw_hit(player.X, player.Y);
		  delay_ms(500);
		  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);
	 }
	 else if(player.X < enemy1.X && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 1 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 5){
		 enemy1.X -= 16;
		 tag = 1;
	 }
	 else if(player.Y < enemy1.Y && occupy[enemy1.X/16][(enemy1.Y-16)/16] != 1 && occupy[enemy1.X/16][(enemy1.Y-16)/16] != 5){
		 enemy1.Y -= 16;
		 tag = 1;
	 }
	 else if(player.X > enemy1.X && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 1 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 5){
		 enemy1.X += 16;
		 tag = 1;
	 }
	 else if(player.Y > enemy1.Y && occupy[enemy1.X/16][(enemy1.Y+16)/16] != 1 && occupy[enemy1.X/16][(enemy1.Y+16)/16] != 5){
		 enemy1.Y += 16;
		 tag = 1;
	 }
	if(tag){
		tag = 0;
		LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, enemy1.image);  
		LCD_DrawRGBImage(enemy1.pre_X, enemy1.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
		occupy[enemy1.X/16][enemy1.Y/16] = 1;				
		occupy[enemy1.pre_X/16][enemy1.pre_Y/16] = 0;		
		enemy1.pre_X = enemy1.X;
		enemy1.pre_Y = enemy1.Y; 
	 }
	 
	 
	 for(int j=0; j < 2 ; j++){
		 if((player.X-16 == enemy2.X && player.Y == enemy2.Y) || (player.X+16 == enemy2.X && player.Y == enemy2.Y)
			 ||(player.Y-16 == enemy2.Y && player.X == enemy2.X) || (player.Y+16 == enemy2.Y && player.X == enemy2.X) ){
			  player.life -= 1;
			  draw_hit(player.X, player.Y);
			  delay_ms(500);
			  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);					
		 } 
		 else if(player.X < enemy2.X && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 1 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 5){
		   enemy2.X -= 16;
		   tag = 1;
		 }
	   else if(player.Y < enemy2.Y && occupy[enemy2.X/16][(enemy2.Y-16)/16] != 1 && occupy[enemy2.X/16][(enemy2.Y-16)/16] != 5){
		   enemy2.Y -= 16;
		   tag = 1;
		 }
		 else if(player.X > enemy2.X && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 1 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 5){
		   enemy2.X += 16;
		   tag = 1;
		 }
		 else if(player.Y > enemy2.Y && occupy[enemy2.X/16][(enemy2.Y+16)/16] != 1 && occupy[enemy2.X/16][(enemy2.Y+16)/16] != 5){
		   enemy2.Y += 16;
		   tag = 1;
		 }
	 
	   if(tag){
			  tag = 0;
			  LCD_DrawRGBImage(enemy2.X, enemy2.Y, 16, 16, enemy2.image);   
			  LCD_DrawRGBImage(enemy2.pre_X, enemy2.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
			  occupy[enemy2.X/16][enemy2.Y/16] = 1;				
			  occupy[enemy2.pre_X/16][enemy2.pre_Y/16] = 0;		
			  enemy2.pre_X = enemy2.X;
			  enemy2.pre_Y = enemy2.Y;				 
	   }				 
	 }			
	 
	  LCD_SetTextColor(RED);
	  sprintf(S1, "%d ", player.life);
	  LCD_DisplayStringAt(295, 40, S1, LEFT_MODE);		
	}
	return player.life;
}

// #-------------------------#
//        state == 3
// #-------------------------#
// 1: enemy, 2: medicine, 3,4: portal 5: feces 6:end
void level_2_draw(void) {
  portal_tag = 0;
	for(int i = 0; i <= 14 ; i++)
		for(int j = 0; j <= 14 ;j++)
			LCD_DrawRGBImage(i*16, j*16, 16, 16, (uint8_t*)0x08010e00); 
	
	player.image = (uint8_t*)0x08010c00;		
	player.hide_image = (uint8_t*)0x08011000;
	LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
	occupy[14][0] = 6;
	LCD_DrawRGBImage(224, 0, 16, 16, (uint8_t*)0x08012200); 
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy1.image = (uint8_t*)0x08012800;
	enemy1.X = tem_x * 16;
	enemy1.Y = tem_y * 16;
	enemy1.pre_X = enemy1.X;
	enemy1.pre_Y = enemy1.Y;
	LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, enemy1.image);  
		
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy2.image = (uint8_t*)0x08012a00;
	enemy2.X = tem_x * 16;
	enemy2.Y = tem_y * 16;
	enemy2.pre_X = enemy2.X;
	enemy2.pre_Y = enemy2.Y;
	LCD_DrawRGBImage(enemy2.X, enemy2.Y, 16, 16, enemy2.image);  

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 3;
	enemy3.image = (uint8_t*)0x08012c00;
	enemy3.X = tem_x * 16;
	enemy3.Y = tem_y * 16;
	enemy3.pre_X = enemy3.X;
	enemy3.pre_Y = enemy3.Y;
	LCD_DrawRGBImage(enemy3.X, enemy3.Y, 16, 16, enemy3.image); 
	
	for(int i = 0; i <= 9; i++){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);
		while(occupy[tem_x][tem_y] != 0){
			tem_x = RNG_Get_RandomRange(1,13);
			tem_y = RNG_Get_RandomRange(1,13);		    
		}		
		occupy[tem_x][tem_y] = 2;
		LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013000);
	}
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 3;
	portal_3.X = tem_x * 16;
	portal_3.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 4;
	portal_4.X = tem_x * 16;
	portal_4.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
}

int level_2_flow(void) {
	tag = 0;
	TSC_TouchDet(1);  
	TS_GetState(&Touch);
	touch_x = -1;
	touch_y = -1;
	if (Touch.TouchDetected){
		touch_x = Touch.x;
		touch_y = Touch.y;
		WaitForTouchRelease(6);
	}

  if(touch_x >= 245 && touch_x <= 309 && touch_y >= 85 && touch_y <= 149 && player.skill == 2){
	  LCD_DrawRGBImage( 245, 85, 64, 64, player.skill_image);
	  player.skill = 1;
	  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);  
  }
  else if(touch_x >= btnUp.X && touch_x <= btnUp.X + 15 && touch_y >= btnUp.Y && touch_y <= btnUp.Y + 25 && player.Y != 0 && occupy[player.X/16][(player.Y-16)/16]!=1){  // up
	 	player.Y -= 16;
	  tag = 1;
	}
  else if(touch_x >= btnDown.X && touch_x <= btnDown.X + 15 && touch_y >= btnDown.Y && touch_y <= btnDown.Y + 25 && player.Y != 224 && occupy[player.X/16][(player.Y+16)/16]!=1){ // down
    player.Y += 16;
	  tag = 1;
  }
  else if(touch_x >= btnLeft.X && touch_x <= btnLeft.X + 25 && touch_y >= btnLeft.Y && touch_y <= btnLeft.Y + 15 && player.X != 0 && occupy[(player.X-16)/16][player.Y/16]!=1){ //left
		player.X -= 16;
		tag = 1;
  } 
  else if(touch_x >= btnRight.X && touch_x <= btnRight.X + 25 && touch_y >= btnRight.Y && touch_y <= btnRight.Y + 15 && player.X != 224 && occupy[(player.X+16)/16][player.Y/16]!=1){ // right
		player.X += 16;
		tag = 1;
  }
 
  if(tag){
		if(player.skill == 0){
			player.skill_round++;
			if(player.skill_round == 4){
				player.skill = 2;
				LCD_DrawRGBImage( 245, 85, 64, 64, player.unskill_image);
			}
		}
		else if(player.skill == 1){
			player.skill_round--;
			if(player.skill_round == 0){
				player.skill = 0;
				LCD_SetTextColor(BLACK);
				LCD_FillRect(245, 85, 64, 64);
				LCD_SetTextColor(WHITE);
			}
		}
			
		if(player.skill != 1)
			 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);  
		else
			 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
		if(portal_tag == 1){
			 LCD_DrawRGBImage(player.pre_X, player.pre_Y, 16, 16, (uint8_t*)0x08013200);  // portal
			 portal_tag = 0;
		}
		else
			 LCD_DrawRGBImage(player.pre_X, player.pre_Y, 16, 16, (uint8_t*)0x08010e00); // reproduction backgroud
		player.pre_X = player.X;
		player.pre_Y = player.Y;
 }
 
 if(tag && occupy[player.X/16][player.Y/16] > 1){
		if(occupy[player.X/16][player.Y/16] == 2){
			draw_heal(player.X, player.Y);
			delay_ms(500);
			if(player.skill == 1)
					LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			else
					LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);
			player.life += 3;
			occupy[player.X/16][player.Y/16] = 0;
		}
		else if(occupy[player.X/16][player.Y/16] == 3){
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			draw_border(player.X, player.Y);
			if(player.skill != 1)
				LCD_DrawRGBImage(portal_4.X, portal_4.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(portal_4.X, portal_4.Y, 16, 16, player.hide_image);
			draw_border(portal_4.X, portal_4.Y);
			delay_ms(500);
			
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal					
			player.X = portal_4.X;
			player.Y = portal_4.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);					
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 4){
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			draw_border(player.X, player.Y);
			if(player.skill != 1)
				LCD_DrawRGBImage(portal_3.X, portal_3.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(portal_3.X, portal_3.Y, 16, 16, player.hide_image);
			draw_border(portal_3.X, portal_3.Y);
			delay_ms(500);					

			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_3.X;
			player.Y = portal_3.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 5){
			player.life--;
			occupy[player.X/16][player.Y/16] = 0;
		}
		else if(occupy[player.X/16][player.Y/16] == 6){
			LCD_SetTextColor(YELLOW);
			LCD_SetFont(&Font16);
			LCD_DisplayStringAt(0, 60, "!! Congratulation Success !!", CENTER_MODE);
			LCD_SetFont(&Font16);					
			LCD_DisplayStringAt(0, 130, "Click to next level", CENTER_MODE);
			LCD_DisplayStringAt(0, 160, "   Click to quit   ", CENTER_MODE);					
			return 1000;
		}
		
		LCD_SetTextColor(RED);
		sprintf(S1, "%d ", player.life);
		LCD_DisplayStringAt(295, 40, S1, LEFT_MODE);					
 }
 
 if(tag && player.skill != 1){
	 tag = 0;
	 for(int j=0; j < 2 ; j++){
		 if((player.X-16 == enemy1.X && player.Y == enemy1.Y) || (player.X+16 == enemy1.X && player.Y == enemy1.Y)
			  ||(player.Y-16 == enemy1.Y && player.X == enemy1.X) || (player.Y+16 == enemy1.Y && player.X == enemy1.X) ){
			 player.life -= 1; 	
			 draw_bite(player.X, player.Y);
			 delay_ms(500);
			 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
		 }
		 else if(player.X < enemy1.X && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 1 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 6 
						 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 3 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 4){
			 enemy1.X -= 16;
			 tag = 1;
		 }
		 else if(player.Y < enemy1.Y && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 1 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 6 
						 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 3 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 4){
			 enemy1.Y -= 16;
			 tag = 1;
		 }
		 else if(player.X > enemy1.X && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 1 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 6 
						 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 3 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 4){
			 enemy1.X += 16;
			 tag = 1;
		 }
		 else if(player.Y > enemy1.Y && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 1 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 6 
						 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 3 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 4){
			 enemy1.Y += 16;
			 tag = 1;
		 }
		if(tag){
			tag = 0;
			LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, enemy1.image);  
			LCD_DrawRGBImage(enemy1.pre_X, enemy1.pre_Y, 16, 16, (uint8_t*)0x08010e00); // reproduction backgroud
			occupy[enemy1.X/16][enemy1.Y/16] = 1;				
			occupy[enemy1.pre_X/16][enemy1.pre_Y/16] = 0;		
			enemy1.pre_X = enemy1.X;
			enemy1.pre_Y = enemy1.Y; 
		 }
	 }
	 
	 for(int j=0; j < 2 ; j++){
		 if((player.X-16 == enemy2.X && player.Y == enemy2.Y) || (player.X+16 == enemy2.X && player.Y == enemy2.Y)
				||(player.Y-16 == enemy2.Y && player.X == enemy2.X) || (player.Y+16 == enemy2.Y && player.X == enemy2.X) ){
			 player.life -= 2; 	
			 draw_bite(player.X, player.Y);
			 delay_ms(500);
			 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
		 }
		 else if(player.X < enemy2.X && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 1 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 6 
						 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 3 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 4){
			 enemy2.X -= 16;
			 tag = 1;
		 }
		 else if(player.Y < enemy2.Y && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 1 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 6 
						 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 3 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 4){
			 enemy2.Y -= 16;
			 tag = 1;
		 }
		 else if(player.X > enemy2.X && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 1 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 6 
						 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 3 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 4){
			 enemy2.X += 16;
			 tag = 1;
		 }
		 else if(player.Y > enemy2.Y && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 1 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 6 
						 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 3 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 4){
			 enemy2.Y += 16;
			 tag = 1;
		 }
		if(tag){
			tag = 0;
			LCD_DrawRGBImage(enemy2.X, enemy2.Y, 16, 16, enemy2.image);  
			LCD_DrawRGBImage(enemy2.pre_X, enemy2.pre_Y, 16, 16, (uint8_t*)0x08010e00); // reproduction backgroud
			occupy[enemy2.X/16][enemy2.Y/16] = 1;				
			occupy[enemy2.pre_X/16][enemy2.pre_Y/16] = 0;		
			enemy2.pre_X = enemy2.X;
			enemy2.pre_Y = enemy2.Y; 
		 }
	 }
	 
	 if((player.X-16 == enemy3.X && player.Y == enemy3.Y) || (player.X+16 == enemy3.X && player.Y == enemy3.Y)
			||(player.Y-16 == enemy3.Y && player.X == enemy3.X) || (player.Y+16 == enemy3.Y && player.X == enemy3.X) ){
			 player.life -= 1; 	
			 draw_bite(player.X, player.Y);
			 delay_ms(500);
			 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
		 }
	 else if(player.X < enemy3.X && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 1 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 6 
					 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 3 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 4){
		 enemy3.X -= 16;
		 tag = 1;
	 }
	 else if(player.Y < enemy3.Y && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 1 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 6 
					 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 3 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 4){
		 enemy3.Y -= 16;
		 tag = 1;
	 }
	 else if(player.X > enemy3.X && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 1 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 6 
					 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 3 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 4){
		 enemy3.X += 16;
		 tag = 1;
	 }
	 else if(player.Y > enemy3.Y && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 1 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 6 
					 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 3 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 4){
		 enemy3.Y += 16;
		 tag = 1;
	 }
	if(tag){
		tag = 0;
		LCD_DrawRGBImage(enemy3.X, enemy3.Y, 16, 16, enemy3.image);  
		LCD_DrawRGBImage(enemy3.pre_X, enemy3.pre_Y, 16, 16, (uint8_t*)0x08012e00); // feces
		occupy[enemy3.X/16][enemy3.Y/16] = 1;				
		occupy[enemy3.pre_X/16][enemy3.pre_Y/16] =  5;		
		enemy3.pre_X = enemy3.X;
		enemy3.pre_Y = enemy3.Y; 
	 }			 
	 
	 LCD_SetTextColor(RED);
	 sprintf(S1, "%d ", player.life);
	 LCD_DisplayStringAt(295, 40, S1, LEFT_MODE);		
	}
	return player.life;
}

// #-------------------------#
//        state == 4
// #-------------------------#
// 1: enermy, 2: stone, 3: time, 4: end
void level_3_draw(void) {
	time_stop = 0;
	for(int i = 0; i <= 14 ; i++)
			for(int j = 0; j <= 14 ;j++)
					LCD_DrawRGBImage(i*16, j*16, 16, 16, (uint8_t*)0x08014000); 

	player.image = (uint8_t*)0x08014200;		
	player.hide_image = (uint8_t*)0x08014400;
	LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
	
	occupy[14][0] = 4;
	occupy[0][0] = 4;
	occupy[14][14] = 4;
	LCD_DrawRGBImage(224, 0, 16, 16, (uint8_t*)0x08013e00); 
	LCD_DrawRGBImage(0, 0, 16, 16, (uint8_t*)0x08013e00); 
	LCD_DrawRGBImage(224, 224, 16, 16, (uint8_t*)0x08013e00); 

	occupy[1][1] = 2; occupy[1][2] = 2; occupy[1][3] = 2; occupy[2][1] = 2; occupy[3][1] = 2; 
	LCD_DrawRGBImage(16, 16, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16, 32, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16, 48, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(32, 16, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(48, 16, 16, 16, (uint8_t*)0x08013a00);

	occupy[13][1] = 2; occupy[13][2] = 2; occupy[13][3] = 2; occupy[12][1] = 2; occupy[11][1] = 2; 
	LCD_DrawRGBImage(16*13, 16, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*13, 32, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*13, 48, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*12, 16, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*11, 16, 16, 16, (uint8_t*)0x08013a00);	

	occupy[13][13] = 2; occupy[13][12] = 2; occupy[13][11] = 2; occupy[12][13] = 2; occupy[11][13] = 2; 
	LCD_DrawRGBImage(16*13, 16*13, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*13, 16*12, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*13, 16*11, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*12, 16*13, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*11, 16*13, 16, 16, (uint8_t*)0x08013a00);

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy1.image = (uint8_t*)0x08013400;
	enemy1.X = tem_x * 16;
	enemy1.Y = tem_y * 16;
	enemy1.pre_X = enemy1.X;
	enemy1.pre_Y = enemy1.Y;
	LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, enemy1.image);  
		
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy2.image = (uint8_t*)0x08013600;
	enemy2.X = tem_x * 16;
	enemy2.Y = tem_y * 16;
	enemy2.pre_X = enemy2.X;
	enemy2.pre_Y = enemy2.Y;
	LCD_DrawRGBImage(enemy2.X, enemy2.Y, 16, 16, enemy2.image);  

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy3.image = (uint8_t*)0x08013800;
	enemy3.X = tem_x * 16;
	enemy3.Y = tem_y * 16;
	enemy3.pre_X = enemy3.X;
	enemy3.pre_Y = enemy3.Y;
	LCD_DrawRGBImage(enemy3.X, enemy3.Y, 16, 16, enemy3.image); 
	

	for(int i = 0; i <= 29; i++){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);
		while(occupy[tem_x][tem_y] != 0){
			tem_x = RNG_Get_RandomRange(1,13);
			tem_y = RNG_Get_RandomRange(1,13);		    
		}		
		occupy[tem_x][tem_y] = 2;
		LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013a00);
	}

	for(int i = 0; i <= 14; i++){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);
		while(occupy[tem_x][tem_y] != 0){
			tem_x = RNG_Get_RandomRange(1,13);
			tem_y = RNG_Get_RandomRange(1,13);		    
		}		
		occupy[tem_x][tem_y] = 3;
		LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013c00);
	}
		
}

int level_3_flow(void) {
	tag = 0;
	TSC_TouchDet(1);  
	TS_GetState(&Touch);
	touch_x = -1;
	touch_y = -1;
	if (Touch.TouchDetected){
		touch_x = Touch.x;
		touch_y = Touch.y;
		WaitForTouchRelease(6);
	}

 if(touch_x >= 245 && touch_x <= 309 && touch_y >= 85 && touch_y <= 149 && player.skill == 2){
	 LCD_DrawRGBImage( 245, 85, 64, 64, player.skill_image);
	 player.skill = 1;
	 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);  
 }
 else if(touch_x >= btnUp.X && touch_x <= btnUp.X + 15 && touch_y >= btnUp.Y && touch_y <= btnUp.Y + 25 && player.Y != 0 
				 && occupy[player.X/16][(player.Y-16)/16]!=1 && occupy[player.X/16][(player.Y-16)/16] != 2){  // up
		player.Y -= 16;
		tag = 1;
	}
 else if(touch_x >= btnDown.X && touch_x <= btnDown.X + 15 && touch_y >= btnDown.Y && touch_y <= btnDown.Y + 25 && player.Y != 224 
				 && occupy[player.X/16][(player.Y+16)/16]!=1 && occupy[player.X/16][(player.Y+16)/16] != 2){ // down
		player.Y += 16;
		tag = 1;
 }
 else if(touch_x >= btnLeft.X && touch_x <= btnLeft.X + 25 && touch_y >= btnLeft.Y && touch_y <= btnLeft.Y + 15 && player.X != 0 
				 && occupy[(player.X-16)/16][player.Y/16]!=1 && occupy[(player.X-16)/16][player.Y/16] != 2){ //left
		player.X -= 16;
		tag = 1;
 } 
 else if(touch_x >= btnRight.X && touch_x <= btnRight.X + 25 && touch_y >= btnRight.Y && touch_y <= btnRight.Y + 15 && player.X != 224 
				 && occupy[(player.X+16)/16][player.Y/16]!=1 && occupy[(player.X+16)/16][player.Y/16] != 2){ // right
		player.X += 16;
		tag = 1;
 }
 
 if(tag){
   if(time_stop > 0)
	   time_stop--;
		
	 if(player.skill == 0){
	   player.skill_round++;
		 if(player.skill_round == 4){
		   player.skill = 2;
			 LCD_DrawRGBImage( 245, 85, 64, 64, player.unskill_image);
		 }
	 }
	 else if(player.skill == 1){
		 player.skill_round--;
		 if(player.skill_round == 0){
			 player.skill = 0;
			 LCD_SetTextColor(BLACK);
			 LCD_FillRect(245, 85, 64, 64);
			 LCD_SetTextColor(WHITE);
		 }
	 }
		
	 if(player.skill != 1)
		  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);  
	 else
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
	 LCD_DrawRGBImage(player.pre_X, player.pre_Y, 16, 16, (uint8_t*)0x08014000); //reproduction backgroud
	 player.pre_X = player.X;
	 player.pre_Y = player.Y;
 }
 
 if(tag && occupy[player.X/16][player.Y/16] > 2){
		if(occupy[player.X/16][player.Y/16] == 3){
			time_stop += 2;
			occupy[player.X/16][player.Y/16] = 0;
		}
		else if(occupy[player.X/16][player.Y/16] == 4){
			LCD_SetTextColor(YELLOW);
			LCD_SetFont(&Font16);
			LCD_DisplayStringAt(0, 60, "!! Congratulation Success !!", CENTER_MODE);
			LCD_SetFont(&Font16);					
			LCD_DisplayStringAt(0, 130, "Click to next level", CENTER_MODE);
			LCD_DisplayStringAt(0, 160, "   Click to quit   ", CENTER_MODE);					
			return 1000;
		}	
 }
	 
 if(tag && player.skill != 1 && time_stop == 0){
	 tag = 0;
	 for(int j=0; j < 2 ; j++){
		 if((player.X-16 == enemy1.X && player.Y == enemy1.Y) || (player.X+16 == enemy1.X && player.Y == enemy1.Y)
				||(player.Y-16 == enemy1.Y && player.X == enemy1.X) || (player.Y+16 == enemy1.Y && player.X == enemy1.X) ){
				 player.life -= 1; 	
				 draw_slash(player.X, player.Y);
				 delay_ms(500);
				 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
			 }
		 else if(player.X < enemy1.X && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 1 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 2 
						 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 4){
			 enemy1.X -= 16;
			 tag = 1;
		 }
		 else if(player.Y < enemy1.Y && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 1 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 2 
						 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 4){
			 enemy1.Y -= 16;
			 tag = 1;
		 }
		 else if(player.X > enemy1.X && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 1 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 2 
						 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 4){
			 enemy1.X += 16;
			 tag = 1;
		 }
		 else if(player.Y > enemy1.Y && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 1 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 2 
						 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 4){
			 enemy1.Y += 16;
			 tag = 1;
		 }
		if(tag){
			tag = 0;
			random = RNG_Get_RandomRange(0,1);
			if(random == 1)
					LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, enemy1.image);  
			else 
					LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, (uint8_t*)0x08014000);    // reproduction backgrouds
			
			LCD_DrawRGBImage(enemy1.pre_X, enemy1.pre_Y, 16, 16, (uint8_t*)0x08014000); // reproduction backgroud
			occupy[enemy1.X/16][enemy1.Y/16] = 1;				
			occupy[enemy1.pre_X/16][enemy1.pre_Y/16] = 0;		
			enemy1.pre_X = enemy1.X;
			enemy1.pre_Y = enemy1.Y; 
		 }
	 }		 
	 
	 for(int j = 0; j < 3 ; j++){
	   if((player.X-16 == enemy2.X && player.Y == enemy2.Y) || (player.X+16 == enemy2.X && player.Y == enemy2.Y)
				||(player.Y-16 == enemy2.Y && player.X == enemy2.X) || (player.Y+16 == enemy2.Y && player.X == enemy2.X) ){
				 player.life -= 1; 	
				 draw_bite(player.X, player.Y);
				 delay_ms(500);
				 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
			 }
		 else if(player.X < enemy2.X && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 1 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 2 
						 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 4){
			 enemy2.X -= 16;
			 tag = 1;
		 }
		 else if(player.Y < enemy2.Y && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 1 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 2 
						 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 4){
			 enemy2.Y -= 16;
			 tag = 1;
		 }
		 else if(player.X > enemy2.X && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 1 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 2 
						 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 4){
			 enemy2.X += 16;
			 tag = 1;
		 }
		 else if(player.Y > enemy2.Y && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 1 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 2 
						 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 4){
			 enemy2.Y += 16;
			 tag = 1;
		 }	 
			if(tag){
				tag = 0;
				LCD_DrawRGBImage(enemy2.X, enemy2.Y, 16, 16, enemy2.image);  
				LCD_DrawRGBImage(enemy2.pre_X, enemy2.pre_Y, 16, 16, (uint8_t*)0x08014000); // reproduction backgroud
				occupy[enemy2.X/16][enemy2.Y/16] = 1;				
				occupy[enemy2.pre_X/16][enemy2.pre_Y/16] = 0;		
				enemy2.pre_X = enemy2.X;
				enemy2.pre_Y = enemy2.Y; 
			 }
		 }

		 if((player.X-16 == enemy3.X && player.Y == enemy3.Y) || (player.X+16 == enemy3.X && player.Y == enemy3.Y)
				||(player.Y-16 == enemy3.Y && player.X == enemy3.X) || (player.Y+16 == enemy3.Y && player.X == enemy3.X) ){
					 draw_splatter(player.X, player.Y);
				 delay_ms(1000);
				 return 0;
			 }
		 else if(player.X < enemy3.X && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 1 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 2 
						 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 4){
			 enemy3.X -= 16;
			 tag = 1;
		 }
		 else if(player.Y < enemy3.Y && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 1 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 2 
						 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 4){
			 enemy3.Y -= 16;
			 tag = 1;
		 }
		 else if(player.X > enemy3.X && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 1 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 2 
						 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 4){
			 enemy3.X += 16;
			 tag = 1;
		 }
		 else if(player.Y > enemy3.Y && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 1 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 2 
						 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 4){
			 enemy3.Y += 16;
			 tag = 1;
		 }	 
		if(tag){
			tag = 0;
			LCD_DrawRGBImage(enemy3.X, enemy3.Y, 16, 16, enemy3.image);  
			LCD_DrawRGBImage(enemy3.pre_X, enemy3.pre_Y, 16, 16, (uint8_t*)0x08014000); // reproduction backgroud
			occupy[enemy3.X/16][enemy3.Y/16] = 1;				
			occupy[enemy3.pre_X/16][enemy3.pre_Y/16] =  0;		
			enemy3.pre_X = enemy3.X;
			enemy3.pre_Y = enemy3.Y; 
		 }			 
		 
		 LCD_SetTextColor(RED);
		 sprintf(S1, "%d ", player.life);
		 LCD_DisplayStringAt(295, 40, S1, LEFT_MODE);		
 }

  if((player.X-16 == enemy3.X && player.Y == enemy3.Y) || (player.X+16 == enemy3.X && player.Y == enemy3.Y)
	    ||(player.Y-16 == enemy3.Y && player.X == enemy3.X) || (player.Y+16 == enemy3.Y && player.X == enemy3.X)){
	  draw_splatter(player.X, player.Y);
	  delay_ms(1000);	
	  return 0;
  }
 return player.life;
}

// #-------------------------#
//        state == 5
// #-------------------------#
// 1: enemy, 2: boom, 3:tornado, 4: blood, 5:end
void level_4_draw(void) {
	for(int i = 0; i <= 14 ; i++){
		for(int j = 0; j <= 14 ;j++){
			LCD_DrawRGBImage(i*16, j*16, 16, 16, (uint8_t*)0x08010800); 
		}
	}
	
	player.image = (uint8_t*)0x08010600;		
	player.hide_image = (uint8_t*)0x08010a00;
	LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
	
	occupy[14][0] = 5;
	LCD_DrawRGBImage(224, 0, 16, 16, (uint8_t*)0x08012000); 
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy1.image = (uint8_t*)0x08011600;
	enemy1.X = tem_x * 16;
	enemy1.Y = tem_y * 16;
	enemy1.pre_X = enemy1.X;
	enemy1.pre_Y = enemy1.Y;
	LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, enemy1.image);  

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy2.image = (uint8_t*)0x08011600;
	enemy2.X = tem_x * 16;
	enemy2.Y = tem_y * 16;
	enemy2.pre_X = enemy2.X;
	enemy2.pre_Y = enemy2.Y;
	LCD_DrawRGBImage(enemy2.X, enemy2.Y, 16, 16, enemy2.image);  

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy3.image = (uint8_t*)0x08011600;
	enemy3.X = tem_x * 16;
	enemy3.Y = tem_y * 16;
	enemy3.pre_X = enemy3.X;
	enemy3.pre_Y = enemy3.Y;
	LCD_DrawRGBImage(enemy3.X, enemy3.Y, 16, 16, enemy3.image); 
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy4.image = (uint8_t*)0x08011600;
	enemy4.X = tem_x * 16;
	enemy4.Y = tem_y * 16;
	enemy4.pre_X = enemy4.X;
	enemy4.pre_Y = enemy4.Y;
	LCD_DrawRGBImage(enemy4.X, enemy4.Y, 16, 16, enemy4.image); 
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy5.image = (uint8_t*)0x08011600;
	enemy5.X = tem_x * 16;
	enemy5.Y = tem_y * 16;
	enemy5.pre_X = enemy5.X;
	enemy5.pre_Y = enemy5.Y;
	LCD_DrawRGBImage(enemy5.X, enemy5.Y, 16, 16, enemy5.image); 
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy6.image = (uint8_t*)0x08011800;
	enemy6.X = tem_x * 16;
	enemy6.Y = tem_y * 16;
	enemy6.pre_X = enemy6.X;
	enemy6.pre_Y = enemy6.Y;
	LCD_DrawRGBImage(enemy6.X, enemy6.Y, 16, 16, enemy6.image); 
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy7.image = (uint8_t*)0x08011800;
	enemy7.X = tem_x * 16;
	enemy7.Y = tem_y * 16;
	enemy7.pre_X = enemy7.X;
	enemy7.pre_Y = enemy7.Y;
	LCD_DrawRGBImage(enemy7.X, enemy7.Y, 16, 16, enemy7.image); 
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy8.image = (uint8_t*)0x08011800;
	enemy8.X = tem_x * 16;
	enemy8.Y = tem_y * 16;
	enemy8.pre_X = enemy8.X;
	enemy8.pre_Y = enemy8.Y;
	LCD_DrawRGBImage(enemy8.X, enemy8.Y, 16, 16, enemy8.image); 
	
	for(int i = 0; i <= 19; i++){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);
		while(occupy[tem_x][tem_y] != 0){
			tem_x = RNG_Get_RandomRange(1,13);
			tem_y = RNG_Get_RandomRange(1,13);		    
		}		
		occupy[tem_x][tem_y] = 2;
		LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08011a00);
	}
	
	for(int i = 0; i <= 24; i++){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);
		while(occupy[tem_x][tem_y] != 0){
			tem_x = RNG_Get_RandomRange(1,13);
			tem_y = RNG_Get_RandomRange(1,13);		    
		}		
		occupy[tem_x][tem_y] = 3;
		LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08011e00);
	}
	
	for(int i = 0; i <= 4; i++){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);
		while(occupy[tem_x][tem_y] != 0){
			tem_x = RNG_Get_RandomRange(1,13);
			tem_y = RNG_Get_RandomRange(1,13) ;		    
		}		
		occupy[tem_x][tem_y] = 4;
		LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08011c00);
	}
}

int level_4_flow(void) {
	tag = 0;
	TSC_TouchDet(1);  
	TS_GetState(&Touch);
	touch_x = -1;
	touch_y = -1;
	if (Touch.TouchDetected){
		touch_x = Touch.x;
		touch_y = Touch.y;
		WaitForTouchRelease(6);
	}

  if(touch_x >= 245 && touch_x <= 309 && touch_y >= 85 && touch_y <= 149 && player.skill == 2){
	  LCD_DrawRGBImage( 245, 85, 64, 64, player.skill_image);
	  player.skill = 1;
	  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);  
  }
  else if(touch_x >= btnUp.X && touch_x <= btnUp.X + 15 && touch_y >= btnUp.Y && touch_y <= btnUp.Y + 25 && player.Y != 0 && occupy[player.X/16][(player.Y-16)/16]!=1){  // up
		player.Y -= 16;
		tag = 1;
	}
  else if(touch_x >= btnDown.X && touch_x <= btnDown.X + 15 && touch_y >= btnDown.Y && touch_y <= btnDown.Y + 25 && player.Y != 224 && occupy[player.X/16][(player.Y+16)/16]!=1){ // down
		player.Y += 16;
	  tag = 1;
  }
  else if(touch_x >= btnLeft.X && touch_x <= btnLeft.X + 25 && touch_y >= btnLeft.Y && touch_y <= btnLeft.Y + 15 && player.X != 0 && occupy[(player.X-16)/16][player.Y/16]!=1){ //left
	  player.X -= 16;
		tag = 1;
  } 
 else if(touch_x >= btnRight.X && touch_x <= btnRight.X + 25 && touch_y >= btnRight.Y && touch_y <= btnRight.Y + 15 && player.X != 224 && occupy[(player.X+16)/16][player.Y/16]!=1){ // right
		player.X += 16;
		tag = 1;
 }
 
 if(tag){
   if(player.skill == 0){
	   player.skill_round++;
		 if(player.skill_round == 4){
		   player.skill = 2;
			 LCD_DrawRGBImage( 245, 85, 64, 64, player.unskill_image);
		 }
	 }
	 else if(player.skill == 1){
		 player.skill_round--;
		 if(player.skill_round == 0){
			 player.skill = 0;
			 LCD_SetTextColor(BLACK);
			 LCD_FillRect(245, 85, 64, 64);
			 LCD_SetTextColor(WHITE);
		 }
	 }
		
	 if(player.skill != 1)
	   LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);  
	 else
		 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);   				
	 LCD_DrawRGBImage(player.pre_X, player.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
	 player.pre_X = player.X;
	 player.pre_Y = player.Y;
 }
 
 if(tag && occupy[player.X/16][player.Y/16] > 1){
		if(occupy[player.X/16][player.Y/16] == 2){
			draw_boom(player.X, player.Y);
			delay_ms(1000);
			return 0;
		}
		else if(occupy[player.X/16][player.Y/16] == 3){
			tem_x = 0;
			tem_y = 0;
			while(occupy[tem_x][tem_y] != 0 || (player.X/16 == tem_x && player.Y/16 == tem_y))
				tem_y++;
			enemy1.X = 0;
			enemy1.Y = tem_y * 16;
			LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, enemy1.image);   
			LCD_DrawRGBImage(enemy1.pre_X, enemy1.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
			occupy[enemy1.X/16][enemy1.Y/16] = 1;				
			occupy[enemy1.pre_X/16][enemy1.pre_Y/16] = 0;
			enemy1.pre_X = enemy1.X;
			enemy1.pre_Y = enemy1.Y; 
			
			while(occupy[tem_x][tem_y] != 0 || (player.X/16 == tem_x && player.Y/16 == tem_y))
				tem_y++;
			enemy2.X = 0;
			enemy2.Y = tem_y * 16;
			LCD_DrawRGBImage(enemy2.X, enemy2.Y, 16, 16, enemy2.image);   
			LCD_DrawRGBImage(enemy2.pre_X, enemy2.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
			occupy[enemy2.X/16][enemy2.Y/16] = 1;				
			occupy[enemy2.pre_X/16][enemy2.pre_Y/16] = 0;
			enemy2.pre_X = enemy2.X;
			enemy2.pre_Y = enemy2.Y; 
			
			while(occupy[tem_x][tem_y] != 0 || (player.X/16 == tem_x && player.Y/16 == tem_y))
				tem_y++;
			enemy3.X = 0;
			enemy3.Y = tem_y * 16;
			LCD_DrawRGBImage(enemy3.X, enemy3.Y, 16, 16, enemy3.image);   
			LCD_DrawRGBImage(enemy3.pre_X, enemy3.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
			occupy[enemy3.X/16][enemy3.Y/16] = 1;				
			occupy[enemy3.pre_X/16][enemy3.pre_Y/16] = 0;
			enemy3.pre_X = enemy3.X;
			enemy3.pre_Y = enemy3.Y; 
			
			tem_x = 1;
			tem_y = 0;
			while(occupy[tem_x][tem_y] != 0 || (player.X/16 == tem_x && player.Y/16 == tem_y))
				tem_x++;
			enemy4.X = tem_x * 16;
			enemy4.Y = 0;
			LCD_DrawRGBImage(enemy4.X, enemy4.Y, 16, 16, enemy4.image);   
			LCD_DrawRGBImage(enemy4.pre_X, enemy4.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
			occupy[enemy4.X/16][enemy4.Y/16] = 1;				
			occupy[enemy4.pre_X/16][enemy4.pre_Y/16] = 0;
			enemy4.pre_X = enemy4.X;
			enemy4.pre_Y = enemy4.Y; 
			
			while(occupy[tem_x][tem_y] != 0 || (player.X/16 == tem_x && player.Y/16 == tem_y))
				tem_x++;
			enemy5.X = tem_x * 16;
			enemy5.Y = 0;
			LCD_DrawRGBImage(enemy5.X, enemy5.Y, 16, 16, enemy5.image);   
			LCD_DrawRGBImage(enemy5.pre_X, enemy5.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
			occupy[enemy5.X/16][enemy5.Y/16] = 1;				
			occupy[enemy5.pre_X/16][enemy5.pre_Y/16] = 0;
			enemy5.pre_X = enemy5.X;
			enemy5.pre_Y = enemy5.Y; 
			
			tem_x = 0;
			tem_y = 14;					
			while(occupy[tem_x][tem_y] != 0 || (player.X/16 == tem_x && player.Y/16 == tem_y))
				tem_x++;
			enemy6.X = tem_x * 16;
			enemy6.Y = 224;
			LCD_DrawRGBImage(enemy6.X, enemy6.Y, 16, 16, enemy6.image); 
			LCD_DrawRGBImage(enemy6.pre_X, enemy6.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
			occupy[enemy6.X/16][enemy6.Y/16] = 1;				
			occupy[enemy6.pre_X/16][enemy6.pre_Y/16] = 0;							
			enemy6.pre_X = enemy6.X;
			enemy6.pre_Y = enemy6.Y; 
				
			while(occupy[tem_x][tem_y] != 0 || (player.X/16 == tem_x && player.Y/16 == tem_y))
				tem_x++;
			enemy7.X = tem_x * 16;
			enemy7.Y = 224;
			LCD_DrawRGBImage(enemy7.X, enemy7.Y, 16, 16, enemy7.image); 
			LCD_DrawRGBImage(enemy7.pre_X, enemy7.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
			occupy[enemy7.X/16][enemy7.Y/16] = 1;				
			occupy[enemy7.pre_X/16][enemy7.pre_Y/16] = 0;							
			enemy7.pre_X = enemy7.X;
			enemy7.pre_Y = enemy7.Y; 					
			
			tem_x = 0;
			tem_y = 14;					
			while(occupy[tem_x][tem_y] != 0 || (player.X/16 == tem_x && player.Y/16 == tem_y))
				tem_y--;
			enemy8.X = 0;
			enemy8.Y = tem_y * 16;
			LCD_DrawRGBImage(enemy8.X, enemy8.Y, 16, 16, enemy8.image); 
			LCD_DrawRGBImage(enemy8.pre_X, enemy8.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
			occupy[enemy8.X/16][enemy8.Y/16] = 1;				
			occupy[enemy8.pre_X/16][enemy8.pre_Y/16] = 0;							
			enemy8.pre_X = enemy8.X;
			enemy8.pre_Y = enemy8.Y; 					
			
			occupy[player.X/16][player.Y/16] = 0;
		}
		else if(occupy[player.X/16][player.Y/16] == 4){
			draw_heal(player.X, player.Y);
			delay_ms(500);
			if(player.skill == 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			else
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);
			player.life += 3;
			occupy[player.X/16][player.Y/16] = 0;
		}
		else if(occupy[player.X/16][player.Y/16] == 5){
			LCD_SetTextColor(YELLOW);
			LCD_SetFont(&Font16);
			LCD_DisplayStringAt(0, 60, "!! Congratulation Success !!", CENTER_MODE);
			LCD_SetFont(&Font16);					
			LCD_DisplayStringAt(0, 130, "Click to next level", CENTER_MODE);
			LCD_DisplayStringAt(0, 160, "   Click to quit   ", CENTER_MODE);					
			return 1000;
		}
		
		LCD_SetTextColor(RED);
		sprintf(S1, "%d ", player.life);
		LCD_DisplayStringAt(295, 40, S1, LEFT_MODE);					
 }
 
 if(tag && player.skill != 1){
	 tag = 0;
	 if((player.X-16 == enemy1.X && player.Y == enemy1.Y) || (player.X+16 == enemy1.X && player.Y == enemy1.Y)
			||(player.Y-16 == enemy1.Y && player.X == enemy1.X) || (player.Y+16 == enemy1.Y && player.X == enemy1.X) ){
		 player.life -= 1; 	
		 draw_hit(player.X, player.Y);
		 delay_ms(500);
		 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
	 }
	 else if(player.X < enemy1.X && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 1 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 5){
		 enemy1.X -= 16;
		 tag = 1;
	 }
	 else if(player.Y < enemy1.Y && occupy[enemy1.X/16][(enemy1.Y-16)/16] != 1 && occupy[enemy1.X/16][(enemy1.Y-16)/16] != 5){
		 enemy1.Y -= 16;
		 tag = 1;
	 }
	 else if(player.X > enemy1.X && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 1 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 5){
		 enemy1.X += 16;
		 tag = 1;
	 }
	 else if(player.Y > enemy1.Y && occupy[enemy1.X/16][(enemy1.Y+16)/16] != 1 && occupy[enemy1.X/16][(enemy1.Y+16)/16] != 5){
		 enemy1.Y += 16;
		 tag = 1;
	 }
	if(tag){
		tag = 0;
		LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, enemy1.image);  
		LCD_DrawRGBImage(enemy1.pre_X, enemy1.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
		occupy[enemy1.X/16][enemy1.Y/16] = 1;				
		occupy[enemy1.pre_X/16][enemy1.pre_Y/16] = 0;		
		enemy1.pre_X = enemy1.X;
		enemy1.pre_Y = enemy1.Y; 
	}
	 
	 if((player.X-16 == enemy2.X && player.Y == enemy2.Y) || (player.X+16 == enemy2.X && player.Y == enemy2.Y)
			||(player.Y-16 == enemy2.Y && player.X == enemy2.X) || (player.Y+16 == enemy2.Y && player.X == enemy2.X) ){
		 player.life -= 1; 	
		 draw_hit(player.X, player.Y);
		 delay_ms(500);
		 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
	 }
	 else if(player.X < enemy2.X && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 1 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 5){
		 enemy2.X -= 16;
		 tag = 1;
	 }
	 else if(player.Y < enemy2.Y && occupy[enemy2.X/16][(enemy2.Y-16)/16] != 1 && occupy[enemy2.X/16][(enemy2.Y-16)/16] != 5){
		 enemy2.Y -= 16;
		 tag = 1;
	 }
	 else if(player.X > enemy2.X && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 1 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 5){
		 enemy2.X += 16;
		 tag = 1;
	 }
	 else if(player.Y > enemy2.Y && occupy[enemy2.X/16][(enemy2.Y+16)/16] != 1 && occupy[enemy2.X/16][(enemy2.Y+16)/16] != 5){
		 enemy2.Y += 16;
		 tag = 1;
	 }
	if(tag){
		tag = 0;
		LCD_DrawRGBImage(enemy2.X, enemy2.Y, 16, 16, enemy2.image);  
		LCD_DrawRGBImage(enemy2.pre_X, enemy2.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
		occupy[enemy2.X/16][enemy2.Y/16] = 1;				
		occupy[enemy2.pre_X/16][enemy2.pre_Y/16] = 0;		
		enemy2.pre_X = enemy2.X;
		enemy2.pre_Y = enemy2.Y; 
	}			 
	 
	 if((player.X-16 == enemy3.X && player.Y == enemy3.Y) || (player.X+16 == enemy3.X && player.Y == enemy3.Y)
			||(player.Y-16 == enemy3.Y && player.X == enemy3.X) || (player.Y+16 == enemy3.Y && player.X == enemy3.X) ){
		 player.life -= 1; 	
		 draw_hit(player.X, player.Y);
		 delay_ms(500);
		 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
	 }
	 else if(player.X < enemy3.X && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 1 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 5){
		 enemy3.X -= 16;
		 tag = 1;
	 }
	 else if(player.Y < enemy3.Y && occupy[enemy3.X/16][(enemy3.Y-16)/16] != 1 && occupy[enemy3.X/16][(enemy3.Y-16)/16] != 5){
		 enemy3.Y -= 16;
		 tag = 1;
	 }
	 else if(player.X > enemy3.X && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 1 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 5){
		 enemy3.X += 16;
		 tag = 1;
	 }
	 else if(player.Y > enemy3.Y && occupy[enemy3.X/16][(enemy3.Y+16)/16] != 1 && occupy[enemy3.X/16][(enemy3.Y+16)/16] != 5){
		 enemy3.Y += 16;
		 tag = 1;
	 }
	if(tag){
		tag = 0;
		LCD_DrawRGBImage(enemy3.X, enemy3.Y, 16, 16, enemy3.image);  
		LCD_DrawRGBImage(enemy3.pre_X, enemy3.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
		occupy[enemy3.X/16][enemy3.Y/16] = 1;				
		occupy[enemy3.pre_X/16][enemy3.pre_Y/16] = 0;		
		enemy3.pre_X = enemy3.X;
		enemy3.pre_Y = enemy3.Y; 
	}			 
	 
	if((player.X-16 == enemy4.X && player.Y == enemy4.Y) || (player.X+16 == enemy4.X && player.Y == enemy4.Y)
			||(player.Y-16 == enemy4.Y && player.X == enemy4.X) || (player.Y+16 == enemy4.Y && player.X == enemy4.X) ){
		 player.life -= 1; 	
		 draw_hit(player.X, player.Y);
		 delay_ms(500);
		 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
	}	
	 else if(player.X < enemy4.X && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 1 && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 5){
		 enemy4.X -= 16;
		 tag = 1;
	 }
	 else if(player.Y < enemy4.Y && occupy[enemy4.X/16][(enemy4.Y-16)/16] != 1 && occupy[enemy4.X/16][(enemy4.Y-16)/16] != 5){
		 enemy4.Y -= 16;
		 tag = 1;
	 }
	 else if(player.X > enemy4.X && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 1 && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 5){
		 enemy4.X += 16;
		 tag = 1;
	 }
	 else if(player.Y > enemy4.Y && occupy[enemy4.X/16][(enemy4.Y+16)/16] != 1 && occupy[enemy4.X/16][(enemy4.Y+16)/16] != 5){
		 enemy4.Y += 16;
		 tag = 1;
	 }
	if(tag){
		tag = 0;
		LCD_DrawRGBImage(enemy4.X, enemy4.Y, 16, 16, enemy4.image);  
		LCD_DrawRGBImage(enemy4.pre_X, enemy4.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
		occupy[enemy4.X/16][enemy4.Y/16] = 1;				
		occupy[enemy4.pre_X/16][enemy4.pre_Y/16] = 0;		
		enemy4.pre_X = enemy4.X;
		enemy4.pre_Y = enemy4.Y; 
	}			
	 
	 if((player.X-16 == enemy5.X && player.Y == enemy5.Y) || (player.X+16 == enemy5.X && player.Y == enemy5.Y)
			||(player.Y-16 == enemy5.Y && player.X == enemy5.X) || (player.Y+16 == enemy5.Y && player.X == enemy5.X) ){
		 player.life -= 1; 	
		 draw_hit(player.X, player.Y);
		 delay_ms(500);
		 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
	 }
	 else if(player.X < enemy5.X && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 1 && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 5){
		 enemy5.X -= 16;
		 tag = 1;
	 }
	 else if(player.Y < enemy5.Y && occupy[enemy5.X/16][(enemy5.Y-16)/16] != 1 && occupy[enemy5.X/16][(enemy5.Y-16)/16] != 5){
		 enemy5.Y -= 16;
		 tag = 1;
	 }
	 else if(player.X > enemy5.X && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 1 && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 5){
		 enemy5.X += 16;
		 tag = 1;
	 }
	 else if(player.Y > enemy5.Y && occupy[enemy5.X/16][(enemy5.Y+16)/16] != 1 && occupy[enemy5.X/16][(enemy5.Y+16)/16] != 5){
		 enemy5.Y += 16;
		 tag = 1;
	 }
	if(tag){
		tag = 0;
		LCD_DrawRGBImage(enemy5.X, enemy5.Y, 16, 16, enemy5.image);  
		LCD_DrawRGBImage(enemy5.pre_X, enemy5.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
		occupy[enemy5.X/16][enemy5.Y/16] = 1;				
		occupy[enemy5.pre_X/16][enemy5.pre_Y/16] = 0;		
		enemy5.pre_X = enemy5.X;
		enemy5.pre_Y = enemy5.Y; 
	}			 
	 
	for(int j=0; j < 2 ; j++){
		if((player.X-16 == enemy6.X && player.Y == enemy6.Y) || (player.X+16 == enemy6.X && player.Y == enemy6.Y)
			 ||(player.Y-16 == enemy6.Y && player.X == enemy6.X) || (player.Y+16 == enemy6.Y && player.X == enemy6.X) ){
		  player.life -= 1; 	
		  draw_hit(player.X, player.Y);
		  delay_ms(500);
		  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
	  }
		else if(player.X < enemy6.X && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 1 && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 5){
		  enemy6.X -= 16;
		  tag = 1;
		}
		else if(player.Y < enemy6.Y && occupy[enemy6.X/16][(enemy6.Y-16)/16] != 1 && occupy[enemy6.X/16][(enemy6.Y-16)/16] != 5){
		  enemy6.Y -= 16;
		  tag = 1;
		}
		else if(player.X > enemy6.X && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 1 && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 5){
		  enemy6.X += 16;
		  tag = 1;
		}
		else if(player.Y > enemy6.Y && occupy[enemy6.X/16][(enemy6.Y+16)/16] != 1 && occupy[enemy6.X/16][(enemy6.Y+16)/16] != 5){
		  enemy6.Y += 16;
		  tag = 1;
		}
	 
	  if(tag){
		  tag = 0;
		  LCD_DrawRGBImage(enemy6.X, enemy6.Y, 16, 16, enemy6.image);   
		  LCD_DrawRGBImage(enemy6.pre_X, enemy6.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
		  occupy[enemy6.X/16][enemy6.Y/16] = 1;				
		  occupy[enemy6.pre_X/16][enemy6.pre_Y/16] = 0;		
		  enemy6.pre_X = enemy6.X;
		  enemy6.pre_Y = enemy6.Y;				 
	  }				 
  }	

	 for(int j=0; j < 2 ; j++){
	 	 if((player.X-16 == enemy7.X && player.Y == enemy7.Y) || (player.X+16 == enemy7.X && player.Y == enemy7.Y)
			 ||(player.Y-16 == enemy7.Y && player.X == enemy7.X) || (player.Y+16 == enemy7.Y && player.X == enemy7.X) ){
		  player.life -= 1; 	
		  draw_hit(player.X, player.Y);
		  delay_ms(500);
		  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
		 }
		 else if(player.X < enemy7.X && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 1 && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 5){
		 	 enemy7.X -= 16;
			 tag = 1;
		 }
		 else if(player.Y < enemy7.Y && occupy[enemy7.X/16][(enemy7.Y-16)/16] != 1 && occupy[enemy7.X/16][(enemy7.Y-16)/16] != 5){
			 enemy7.Y -= 16;
			 tag = 1;
		 }
		 else if(player.X > enemy7.X && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 1 && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 5){
			 enemy7.X += 16;
			 tag = 1;
		 }
		 else if(player.Y > enemy7.Y && occupy[enemy7.X/16][(enemy7.Y+16)/16] != 1 && occupy[enemy7.X/16][(enemy7.Y+16)/16] != 5){
			 enemy7.Y += 16;
			 tag = 1;
		 }
		 
		 if(tag){
			 tag = 0;
			 LCD_DrawRGBImage(enemy7.X, enemy7.Y, 16, 16, enemy7.image);   
			 LCD_DrawRGBImage(enemy7.pre_X, enemy7.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
			 occupy[enemy7.X/16][enemy7.Y/16] = 1;				
			 occupy[enemy7.pre_X/16][enemy7.pre_Y/16] = 0;		
			 enemy7.pre_X = enemy7.X;
			 enemy7.pre_Y = enemy7.Y;				 
		 }				 
	 }

	 for(int j=0; j < 2 ; j++){
			if((player.X-16 == enemy8.X && player.Y == enemy8.Y) || (player.X+16 == enemy8.X && player.Y == enemy8.Y)
				 ||(player.Y-16 == enemy8.Y && player.X == enemy8.X) || (player.Y+16 == enemy8.Y && player.X == enemy8.X) ){
			  player.life -= 1; 	
			  draw_hit(player.X, player.Y);
			  delay_ms(500);
			  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
		 }
			else if(player.X < enemy8.X && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 1 && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 5){
			  enemy8.X -= 16;
			  tag = 1;
			}
			else if(player.Y < enemy8.Y && occupy[enemy8.X/16][(enemy8.Y-16)/16] != 1 && occupy[enemy8.X/16][(enemy8.Y-16)/16] != 5){
			  enemy8.Y -= 16;
			  tag = 1;
			}
			else if(player.X > enemy8.X && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 1 && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 5){
			  enemy8.X += 16;
			  tag = 1;
			}
			else if(player.Y > enemy8.Y && occupy[enemy8.X/16][(enemy8.Y+16)/16] != 1 && occupy[enemy8.X/16][(enemy8.Y+16)/16] != 5){
			  enemy8.Y += 16;
			  tag = 1;
			}
		 
		 if(tag){
				tag = 0;
				LCD_DrawRGBImage(enemy8.X, enemy8.Y, 16, 16, enemy8.image);   
				LCD_DrawRGBImage(enemy8.pre_X, enemy8.pre_Y, 16, 16, (uint8_t*)0x08010800); // reproduction backgroud
				occupy[enemy8.X/16][enemy8.Y/16] = 1;				
				occupy[enemy8.pre_X/16][enemy8.pre_Y/16] = 0;		
				enemy8.pre_X = enemy8.X;
				enemy8.pre_Y = enemy8.Y;				 
		 }				 
	 }			 
	 LCD_SetTextColor(RED);
	 sprintf(S1, "%d ", player.life);
	 LCD_DisplayStringAt(295, 40, S1, LEFT_MODE);		
	}
	return player.life;
}

// #-------------------------#
//        state == 6
// #-------------------------#
// 1: enemy, 2: medicine, 5: feces 6:end, 11~20 : portol

void level_5_draw(void) {
	portal_tag = 0;

	for(int i = 0; i <= 14 ; i++)
		for(int j = 0; j <= 14 ;j++)
			LCD_DrawRGBImage(i*16, j*16, 16, 16, (uint8_t*)0x08010e00); 
	
	player.image = (uint8_t*)0x08010c00;		
	player.hide_image = (uint8_t*)0x08011000;
	LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
	
	occupy[14][0] = 6;
	LCD_DrawRGBImage(224, 0, 16, 16, (uint8_t*)0x08012200); 
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy1.image = (uint8_t*)0x08012800;
	enemy1.X = tem_x * 16;
	enemy1.Y = tem_y * 16;
	enemy1.pre_X = enemy1.X;
	enemy1.pre_Y = enemy1.Y;
	LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, enemy1.image);  
		
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy2.image = (uint8_t*)0x08012a00;
	enemy2.X = tem_x * 16;
	enemy2.Y = tem_y * 16;
	enemy2.pre_X = enemy2.X;
	enemy2.pre_Y = enemy2.Y;
	LCD_DrawRGBImage(enemy2.X, enemy2.Y, 16, 16, enemy2.image);  

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy3.image = (uint8_t*)0x08012c00;
	enemy3.X = tem_x * 16;
	enemy3.Y = tem_y * 16;
	enemy3.pre_X = enemy3.X;
	enemy3.pre_Y = enemy3.Y;
	LCD_DrawRGBImage(enemy3.X, enemy3.Y, 16, 16, enemy3.image); 

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy4.image = (uint8_t*)0x08012c00;
	enemy4.X = tem_x * 16;
	enemy4.Y = tem_y * 16;
	enemy4.pre_X = enemy4.X;
	enemy4.pre_Y = enemy4.Y;
	LCD_DrawRGBImage(enemy4.X, enemy4.Y, 16, 16, enemy4.image); 
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy5.image = (uint8_t*)0x08012c00;
	enemy5.X = tem_x * 16;
	enemy5.Y = tem_y * 16;
	enemy5.pre_X = enemy5.X;
	enemy5.pre_Y = enemy5.Y;
	LCD_DrawRGBImage(enemy5.X, enemy5.Y, 16, 16, enemy5.image); 

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy6.image = (uint8_t*)0x08012c00;
	enemy6.X = tem_x * 16;
	enemy6.Y = tem_y * 16;
	enemy6.pre_X = enemy6.X;
	enemy6.pre_Y = enemy6.Y;
	LCD_DrawRGBImage(enemy6.X, enemy6.Y, 16, 16, enemy6.image); 

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy7.image = (uint8_t*)0x08012c00;
	enemy7.X = tem_x * 16;
	enemy7.Y = tem_y * 16;
	enemy7.pre_X = enemy7.X;
	enemy7.pre_Y = enemy7.Y;
	LCD_DrawRGBImage(enemy7.X, enemy7.Y, 16, 16, enemy7.image); 

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy8.image = (uint8_t*)0x08012c00;
	enemy8.X = tem_x * 16;
	enemy8.Y = tem_y * 16;
	enemy8.pre_X = enemy8.X;
	enemy8.pre_Y = enemy8.Y;
	LCD_DrawRGBImage(enemy8.X, enemy8.Y, 16, 16, enemy8.image); 

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy9.image = (uint8_t*)0x08012c00;
	enemy9.X = tem_x * 16;
	enemy9.Y = tem_y * 16;
	enemy9.pre_X = enemy9.X;
	enemy9.pre_Y = enemy9.Y;
	LCD_DrawRGBImage(enemy9.X, enemy9.Y, 16, 16, enemy9.image); 	
	
	for(int i = 0; i <= 5; i++){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);
		while(occupy[tem_x][tem_y] != 0){
			tem_x = RNG_Get_RandomRange(1,13);
			tem_y = RNG_Get_RandomRange(1,13);		    
		}		
		occupy[tem_x][tem_y] = 2;
		LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013000);
	}

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 11;
	portal_1.X = tem_x * 16;
	portal_1.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);		
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 12;
	portal_2.X = tem_x * 16;
	portal_2.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 13;
	portal_3.X = tem_x * 16;
	portal_3.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 14;
	portal_4.X = tem_x * 16;
	portal_4.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 15;
	portal_5.X = tem_x * 16;
	portal_5.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 16;
	portal_6.X = tem_x * 16;
	portal_6.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 17;
	portal_7.X = tem_x * 16;
	portal_7.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 18;
	portal_8.X = tem_x * 16;
	portal_8.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 19;
	portal_9.X = tem_x * 16;
	portal_9.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 20;
	portal_10.X = tem_x * 16;
	portal_10.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
		
}

int level_5_flow(void) {

	tag = 0;
	TSC_TouchDet(1);  
	TS_GetState(&Touch);
	touch_x = -1;
	touch_y = -1;
	if (Touch.TouchDetected){
		touch_x = Touch.x;
		touch_y = Touch.y;
		WaitForTouchRelease(6);
	}

  if(touch_x >= 245 && touch_x <= 309 && touch_y >= 85 && touch_y <= 149 && player.skill == 2){
	  LCD_DrawRGBImage( 245, 85, 64, 64, player.skill_image);
	  player.skill = 1;
	  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);  
  }
  else if(touch_x >= btnUp.X && touch_x <= btnUp.X + 15 && touch_y >= btnUp.Y && touch_y <= btnUp.Y + 25 && player.Y != 0 && occupy[player.X/16][(player.Y-16)/16]!=1){  // up
	  player.Y -= 16;
		tag = 1;
	}
  else if(touch_x >= btnDown.X && touch_x <= btnDown.X + 15 && touch_y >= btnDown.Y && touch_y <= btnDown.Y + 25 && player.Y != 224 && occupy[player.X/16][(player.Y+16)/16]!=1){ // down
		player.Y += 16;
		tag = 1;
  }
  else if(touch_x >= btnLeft.X && touch_x <= btnLeft.X + 25 && touch_y >= btnLeft.Y && touch_y <= btnLeft.Y + 15 && player.X != 0 && occupy[(player.X-16)/16][player.Y/16]!=1){ //left
		player.X -= 16;
		tag = 1;
  } 
  else if(touch_x >= btnRight.X && touch_x <= btnRight.X + 25 && touch_y >= btnRight.Y && touch_y <= btnRight.Y + 15 && player.X != 224 && occupy[(player.X+16)/16][player.Y/16]!=1){ // right
		player.X += 16;
		tag = 1;
  }
 
  if(tag){
	  if(player.skill == 0){
		  player.skill_round++;
		  if(player.skill_round == 4){
			  player.skill = 2;
			  LCD_DrawRGBImage( 245, 85, 64, 64, player.unskill_image);
		  }
	  }
	  else if(player.skill == 1){
		  player.skill_round--;
		  if(player.skill_round == 0){
			  player.skill = 0;
			  LCD_SetTextColor(BLACK);
			  LCD_FillRect(245, 85, 64, 64);
			  LCD_SetTextColor(WHITE);
		  }
	  }
	
	  if(player.skill != 1)
		  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);  
	  else
		  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
	  if(portal_tag == 1){
		  LCD_DrawRGBImage(player.pre_X, player.pre_Y, 16, 16, (uint8_t*)0x08013200);  // portal
		  portal_tag = 0;
	  }
	  else
		  LCD_DrawRGBImage(player.pre_X, player.pre_Y, 16, 16, (uint8_t*)0x08010e00); // reproduction backgroud
	  player.pre_X = player.X;
	  player.pre_Y = player.Y;
  }
 
 if(tag && occupy[player.X/16][player.Y/16] > 1){
		if(occupy[player.X/16][player.Y/16] == 2){
			draw_heal(player.X, player.Y);
			delay_ms(500);
			if(player.skill == 1)
					LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			else
					LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);
			player.life += 3;
			occupy[player.X/16][player.Y/16] = 0;
		}
		else if(occupy[player.X/16][player.Y/16] == 11){
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			draw_border(player.X, player.Y);
			if(player.skill != 1)
				LCD_DrawRGBImage(portal_2.X, portal_2.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(portal_2.X, portal_2.Y, 16, 16, player.hide_image);
			draw_border(portal_2.X, portal_2.Y);
			delay_ms(500);					
			
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_2.X;
			player.Y = portal_2.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 12){
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			draw_border(player.X, player.Y);
			if(player.skill != 1)
				LCD_DrawRGBImage(portal_1.X, portal_1.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(portal_1.X, portal_1.Y, 16, 16, player.hide_image);
			draw_border(portal_1.X, portal_1.Y);
			delay_ms(500);						
			
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_1.X;
			player.Y = portal_1.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 13){
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			draw_border(player.X, player.Y);
			if(player.skill != 1)
				LCD_DrawRGBImage(portal_4.X, portal_4.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(portal_4.X, portal_4.Y, 16, 16, player.hide_image);
			draw_border(portal_4.X, portal_4.Y);
			delay_ms(500);						
			
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_4.X;
			player.Y = portal_4.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 14){
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			draw_border(player.X, player.Y);
			if(player.skill != 1)
				LCD_DrawRGBImage(portal_3.X, portal_3.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(portal_3.X, portal_3.Y, 16, 16, player.hide_image);
			draw_border(portal_3.X, portal_3.Y);
			delay_ms(500);						
			
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_3.X;
			player.Y = portal_3.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 15){
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			draw_border(player.X, player.Y);
			if(player.skill != 1)
				LCD_DrawRGBImage(portal_6.X, portal_6.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(portal_6.X, portal_6.Y, 16, 16, player.hide_image);
			draw_border(portal_6.X, portal_6.Y);
			delay_ms(500);						
			
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_6.X;
			player.Y = portal_6.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 16){
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			draw_border(player.X, player.Y);
			if(player.skill != 1)
				LCD_DrawRGBImage(portal_5.X, portal_5.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(portal_5.X, portal_5.Y, 16, 16, player.hide_image);
			draw_border(portal_5.X, portal_5.Y);
			delay_ms(500);						
			
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_5.X;
			player.Y = portal_5.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 17){
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			draw_border(player.X, player.Y);
			if(player.skill != 1)
				LCD_DrawRGBImage(portal_8.X, portal_8.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(portal_8.X, portal_8.Y, 16, 16, player.hide_image);
			draw_border(portal_8.X, portal_8.Y);
			delay_ms(500);						
			
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_8.X;
			player.Y = portal_8.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 18){
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			draw_border(player.X, player.Y);
			if(player.skill != 1)
				LCD_DrawRGBImage(portal_7.X, portal_7.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(portal_7.X, portal_7.Y, 16, 16, player.hide_image);
			draw_border(portal_7.X, portal_7.Y);
			delay_ms(500);						
			
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_7.X;
			player.Y = portal_7.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 19){
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			draw_border(player.X, player.Y);
			if(player.skill != 1)
				LCD_DrawRGBImage(portal_10.X, portal_10.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(portal_10.X, portal_10.Y, 16, 16, player.hide_image);
			draw_border(portal_10.X, portal_10.Y);
			delay_ms(500);						
			
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_10.X;
			player.Y = portal_10.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 20){
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			draw_border(player.X, player.Y);
			if(player.skill != 1)
				LCD_DrawRGBImage(portal_9.X, portal_9.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(portal_9.X, portal_9.Y, 16, 16, player.hide_image);
			draw_border(portal_9.X, portal_9.Y);
			delay_ms(500);						
			
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_9.X;
			player.Y = portal_9.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 5){
			player.life--;
			occupy[player.X/16][player.Y/16] = 0;
		}
		else if(occupy[player.X/16][player.Y/16] == 6){
			LCD_SetTextColor(YELLOW);
			LCD_SetFont(&Font16);
			LCD_DisplayStringAt(0, 60, "!! Congratulation Success !!", CENTER_MODE);
			LCD_SetFont(&Font16);					
			LCD_DisplayStringAt(0, 130, "Click to next level", CENTER_MODE);
			LCD_DisplayStringAt(0, 160, "   Click to quit   ", CENTER_MODE);					
			return 1000;
		}
		
		LCD_SetTextColor(RED);
		sprintf(S1, "%d ", player.life);
		LCD_DisplayStringAt(295, 40, S1, LEFT_MODE);					
 }
 
 if(tag && player.skill != 1){
	 tag = 0;
	 for(int j=0; j < 2 ; j++){
		 if((player.X-16 == enemy1.X && player.Y == enemy1.Y) || (player.X+16 == enemy1.X && player.Y == enemy1.Y)
				||(player.Y-16 == enemy1.Y && player.X == enemy1.X) || (player.Y+16 == enemy1.Y && player.X == enemy1.X) ){
			 player.life -= 1; 	
			 draw_bite(player.X, player.Y);
			 delay_ms(500);
			 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
		 }
		 else if(player.X < enemy1.X && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 1 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 6 
						 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 11 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 12
						 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 13 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 14
						 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 15 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 16
						 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 17 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 18
						 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 19 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 20){
			 enemy1.X -= 16;
			 tag = 1;
		 }
		 else if(player.Y < enemy1.Y && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 1 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 6 
						 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 11 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 12
						 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 13 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 14
						 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 15 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 16
						 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 17 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 18
						 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 19 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 20){
			 enemy1.Y -= 16;
			 tag = 1;
		 }
		 else if(player.X > enemy1.X && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 1 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 6 
						 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 11 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 12
						 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 13 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 14
						 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 15 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 16
						 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 17 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 18
						 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 19 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 20){
			 enemy1.X += 16;
			 tag = 1;
		 }
		 else if(player.Y > enemy1.Y && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 1 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 6 
						 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 11 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 12
						 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 13 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 14
						 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 15 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 16
						 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 17 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 18
						 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 19 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 20){
			 enemy1.Y += 16;
			 tag = 1;
		 }
		if(tag){
			tag = 0;
			LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, enemy1.image);  
			LCD_DrawRGBImage(enemy1.pre_X, enemy1.pre_Y, 16, 16, (uint8_t*)0x08010e00); // reproduction backgroud
			occupy[enemy1.X/16][enemy1.Y/16] = 1;				
			occupy[enemy1.pre_X/16][enemy1.pre_Y/16] = 0;		
			enemy1.pre_X = enemy1.X;
			enemy1.pre_Y = enemy1.Y; 
		}
  }
	 
	for(int j=0; j < 2 ; j++){
		if((player.X-16 == enemy2.X && player.Y == enemy2.Y) || (player.X+16 == enemy2.X && player.Y == enemy2.Y)
				||(player.Y-16 == enemy2.Y && player.X == enemy2.X) || (player.Y+16 == enemy2.Y && player.X == enemy2.X) ){
			player.life -= 2; 	
			draw_bite(player.X, player.Y);
			delay_ms(500);
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
		}
		else if(player.X < enemy2.X && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 1 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 6 
						 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 11 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 12
						 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 13 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 14
						 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 15 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 16
						 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 17 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 18
						 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 19 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 20){
			enemy2.X -= 16;
			tag = 1;
		}
		else if(player.Y < enemy2.Y && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 1 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 6 
						 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 11 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 12
						 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 13 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 14
						 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 15 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 16
						 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 17 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 18
						 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 19 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 20){
		  enemy2.Y -= 16;
			tag = 1;
		}
		else if(player.X > enemy2.X && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 1 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 6 
						 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 11 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 12
						 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 13 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 14
						 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 15 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 16
						 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 17 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 18
						 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 19 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 20){
			 enemy2.X += 16;
			 tag = 1;
		 }
		 else if(player.Y > enemy2.Y && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 1 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 6 
						 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 11 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 12
						 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 13 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 14
						 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 15 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 16
						 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 17 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 18
						 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 19 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 20){
			 enemy2.Y += 16;
			 tag = 1;
		 }
		if(tag){
			 tag = 0;
			 LCD_DrawRGBImage(enemy2.X, enemy2.Y, 16, 16, enemy2.image);  
			 LCD_DrawRGBImage(enemy2.pre_X, enemy2.pre_Y, 16, 16, (uint8_t*)0x08010e00); // reproduction backgroud
			 occupy[enemy2.X/16][enemy2.Y/16] = 1;				
			 occupy[enemy2.pre_X/16][enemy2.pre_Y/16] = 0;		
			 enemy2.pre_X = enemy2.X;
			 enemy2.pre_Y = enemy2.Y; 
		 }
	 }
	 
	 if((player.X-16 == enemy3.X && player.Y == enemy3.Y) || (player.X+16 == enemy3.X && player.Y == enemy3.Y)
			||(player.Y-16 == enemy3.Y && player.X == enemy3.X) || (player.Y+16 == enemy3.Y && player.X == enemy3.X) ){
			 player.life -= 1; 	
			 draw_bite(player.X, player.Y);
			 delay_ms(500);
			 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
	 }
	 else if(player.X < enemy3.X && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 1 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 6 
					 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 11 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 12
					 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 13 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 14
					 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 15 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 16
					 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 17 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 18
					 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 19 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 20){
		 enemy3.X -= 16;
		 tag = 1;
	 }
	 else if(player.Y < enemy3.Y && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 1 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 6 
					 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 11 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 12
					 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 13 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 14
					 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 15 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 16
					 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 17 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 18
					 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 19 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 20){
		 enemy3.Y -= 16;
		 tag = 1;
	 }
	 else if(player.X > enemy3.X && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 1 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 6 
					 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 11 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 12
					 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 13 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 14
					 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 15 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 16
					 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 17 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 18
					 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 19 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 20){
		 enemy3.X += 16;
		 tag = 1;
	 }
	 else if(player.Y > enemy3.Y && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 1 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 6 
					 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 11 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 12
					 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 13 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 14
					 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 15 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 16
					 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 17 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 18
					 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 19 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 20){
		 enemy3.Y += 16;
		 tag = 1;
	 }
	if(tag){
	  tag = 0;
		LCD_DrawRGBImage(enemy3.X, enemy3.Y, 16, 16, enemy3.image);  
		LCD_DrawRGBImage(enemy3.pre_X, enemy3.pre_Y, 16, 16, (uint8_t*)0x08012e00); // feces
		occupy[enemy3.X/16][enemy3.Y/16] = 1;				
		occupy[enemy3.pre_X/16][enemy3.pre_Y/16] =  5;		
		enemy3.pre_X = enemy3.X;
		enemy3.pre_Y = enemy3.Y; 
	}			 

	 if((player.X-16 == enemy4.X && player.Y == enemy4.Y) || (player.X+16 == enemy4.X && player.Y == enemy4.Y)
			||(player.Y-16 == enemy4.Y && player.X == enemy4.X) || (player.Y+16 == enemy4.Y && player.X == enemy4.X) ){
	   player.life -= 1; 	
		 draw_bite(player.X, player.Y);
		 delay_ms(500);
		 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
	 }	
	 else if(player.X < enemy4.X && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 1 && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 6 
					 && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 11 && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 12
					 && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 13 && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 14
					 && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 15 && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 16
					 && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 17 && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 18
					 && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 19 && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 20){
		 enemy4.X -= 16;
		 tag = 1;
	 }
	 else if(player.Y < enemy4.Y && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 1 && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 6 
					 && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 11 && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 12
					 && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 13 && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 14
					 && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 15 && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 16
					 && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 17 && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 18
					 && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 19 && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 20){
		 enemy4.Y -= 16;
		 tag = 1;
	 }
	 else if(player.X > enemy4.X && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 1 && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 6 
					 && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 11 && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 12
					 && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 13 && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 14
					 && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 15 && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 16
					 && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 17 && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 18
					 && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 19 && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 20){
		 enemy4.X += 16;
		 tag = 1;
	 }
	 else if(player.Y > enemy4.Y && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 1 && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 6 
					 && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 11 && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 12
					 && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 13 && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 14
					 && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 15 && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 16
					 && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 17 && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 18
					 && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 19 && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 20){
		 enemy4.Y += 16;
		 tag = 1;
	 }
	if(tag){
		tag = 0;
		LCD_DrawRGBImage(enemy4.X, enemy4.Y, 16, 16, enemy4.image);  
		LCD_DrawRGBImage(enemy4.pre_X, enemy4.pre_Y, 16, 16, (uint8_t*)0x08012e00); // feces
		occupy[enemy4.X/16][enemy4.Y/16] = 1;				
		occupy[enemy4.pre_X/16][enemy4.pre_Y/16] =  5;		
		enemy4.pre_X = enemy4.X;
		enemy4.pre_Y = enemy4.Y; 
	}

	 if((player.X-16 == enemy5.X && player.Y == enemy5.Y) || (player.X+16 == enemy5.X && player.Y == enemy5.Y)
			||(player.Y-16 == enemy5.Y && player.X == enemy5.X) || (player.Y+16 == enemy5.Y && player.X == enemy5.X) ){
		 player.life -= 1; 	
		 draw_bite(player.X, player.Y);
		 delay_ms(500);
		 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
	 }
	 else if(player.X < enemy5.X && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 1 && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 6 
					 && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 11 && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 12
					 && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 13 && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 14
					 && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 15 && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 16
					 && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 17 && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 18
					 && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 19 && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 20){
		 enemy5.X -= 16;
		 tag = 1;
	 }
	 else if(player.Y < enemy5.Y && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 1 && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 6 
					 && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 11 && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 12
					 && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 13 && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 14
					 && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 15 && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 16
					 && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 17 && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 18
					 && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 19 && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 20){
		 enemy5.Y -= 16;
		 tag = 1;
	 }
	 else if(player.X > enemy5.X && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 1 && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 6 
					 && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 11 && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 12
					 && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 13 && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 14
					 && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 15 && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 16
					 && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 17 && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 18
					 && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 19 && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 20){
		 enemy5.X += 16;
		 tag = 1;
	 }
	 else if(player.Y > enemy5.Y && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 1 && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 6 
					 && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 11 && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 12
					 && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 13 && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 14
					 && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 15 && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 16
					 && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 17 && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 18
					 && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 19 && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 20){
		 enemy5.Y += 16;
		 tag = 1;
	 }
	if(tag){
		tag = 0;
		LCD_DrawRGBImage(enemy5.X, enemy5.Y, 16, 16, enemy5.image);  
		LCD_DrawRGBImage(enemy5.pre_X, enemy5.pre_Y, 16, 16, (uint8_t*)0x08012e00); // feces
		occupy[enemy5.X/16][enemy5.Y/16] = 1;				
		occupy[enemy5.pre_X/16][enemy5.pre_Y/16] =  5;		
		enemy5.pre_X = enemy5.X;
		enemy5.pre_Y = enemy5.Y; 
	}
	 
	 if((player.X-16 == enemy6.X && player.Y == enemy6.Y) || (player.X+16 == enemy6.X && player.Y == enemy6.Y)
			||(player.Y-16 == enemy6.Y && player.X == enemy6.X) || (player.Y+16 == enemy6.Y && player.X == enemy6.X) ){
		 player.life -= 1; 	
		 draw_bite(player.X, player.Y);
		 delay_ms(500);
		 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
	 }
	 else if(player.X < enemy6.X && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 1 && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 6 
					 && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 11 && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 12
					 && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 13 && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 14
					 && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 15 && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 16
					 && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 17 && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 18
					 && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 19 && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 20){
		 enemy6.X -= 16;
		 tag = 1;
	 }
	 else if(player.Y < enemy6.Y && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 1 && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 6 
					 && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 11 && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 12
					 && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 13 && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 14
					 && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 15 && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 16
					 && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 17 && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 18
					 && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 19 && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 20){
		 enemy6.Y -= 16;
		 tag = 1;
	 }
	 else if(player.X > enemy6.X && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 1 && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 6 
					 && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 11 && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 12
					 && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 13 && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 14
					 && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 15 && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 16
					 && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 17 && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 18
					 && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 19 && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 20){
		 enemy6.X += 16;
		 tag = 1;
	 }
	 else if(player.Y > enemy6.Y && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 1 && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 6 
					 && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 11 && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 12
					 && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 13 && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 14
					 && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 15 && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 16
					 && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 17 && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 18
					 && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 19 && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 20){
		 enemy6.Y += 16;
		 tag = 1;
	 }
	if(tag){
		tag = 0;
		LCD_DrawRGBImage(enemy6.X, enemy6.Y, 16, 16, enemy6.image);  
		LCD_DrawRGBImage(enemy6.pre_X, enemy6.pre_Y, 16, 16, (uint8_t*)0x08012e00); // feces
		occupy[enemy6.X/16][enemy6.Y/16] = 1;				
		occupy[enemy6.pre_X/16][enemy6.pre_Y/16] =  5;		
		enemy6.pre_X = enemy6.X;
		enemy6.pre_Y = enemy6.Y; 
	}			 
	 
	 if((player.X-16 == enemy7.X && player.Y == enemy7.Y) || (player.X+16 == enemy7.X && player.Y == enemy7.Y)
			||(player.Y-16 == enemy7.Y && player.X == enemy7.X) || (player.Y+16 == enemy7.Y && player.X == enemy7.X) ){
		 player.life -= 1; 	
		 draw_bite(player.X, player.Y);
		 delay_ms(500);
		 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
	 }
	 else if(player.X < enemy7.X && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 1 && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 6 
					 && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 11 && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 12
					 && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 13 && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 14
					 && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 15 && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 16
					 && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 17 && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 18
					 && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 19 && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 20){
		 enemy7.X -= 16;
		 tag = 1;
	 }
	 else if(player.Y < enemy7.Y && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 1 && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 6 
					 && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 11 && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 12
					 && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 13 && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 14
					 && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 15 && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 16
					 && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 17 && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 18
					 && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 19 && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 20){
		 enemy7.Y -= 16;
		 tag = 1;
	 }
	 else if(player.X > enemy7.X && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 1 && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 6 
					 && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 11 && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 12
					 && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 13 && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 14
					 && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 15 && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 16
					 && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 17 && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 18
					 && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 19 && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 20){
		 enemy7.X += 16;
		 tag = 1;
	 }
	 else if(player.Y > enemy7.Y && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 1 && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 6 
					 && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 11 && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 12
					 && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 13 && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 14
					 && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 15 && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 16
					 && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 17 && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 18
					 && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 19 && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 20){
		 enemy7.Y += 16;
		 tag = 1;
	 }
	if(tag){
		tag = 0;
		LCD_DrawRGBImage(enemy7.X, enemy7.Y, 16, 16, enemy7.image);  
		LCD_DrawRGBImage(enemy7.pre_X, enemy7.pre_Y, 16, 16, (uint8_t*)0x08012e00); // feces
		occupy[enemy7.X/16][enemy7.Y/16] = 1;				
		occupy[enemy7.pre_X/16][enemy7.pre_Y/16] =  5;		
		enemy7.pre_X = enemy7.X;
		enemy7.pre_Y = enemy7.Y; 
	}			 
	 
	 if((player.X-16 == enemy8.X && player.Y == enemy8.Y) || (player.X+16 == enemy8.X && player.Y == enemy8.Y)
			||(player.Y-16 == enemy8.Y && player.X == enemy8.X) || (player.Y+16 == enemy8.Y && player.X == enemy8.X) ){
		 player.life -= 1; 	
		 draw_bite(player.X, player.Y);
		 delay_ms(500);
		 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
	 }
	 else if(player.X < enemy8.X && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 1 && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 6 
					 && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 11 && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 12
					 && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 13 && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 14
					 && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 15 && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 16
					 && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 17 && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 18
					 && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 19 && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 20){
		 enemy8.X -= 16;
		 tag = 1;
	 }
	 else if(player.Y < enemy8.Y && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 1 && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 6 
					 && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 11 && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 12
					 && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 13 && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 14
					 && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 15 && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 16
					 && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 17 && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 18
					 && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 19 && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 20){
		 enemy8.Y -= 16;
		 tag = 1;
	 }
	 else if(player.X > enemy8.X && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 1 && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 6 
					 && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 11 && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 12
					 && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 13 && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 14
					 && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 15 && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 16
					 && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 17 && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 18
					 && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 19 && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 20){
		 enemy8.X += 16;
		 tag = 1;
	 }
	 else if(player.Y > enemy8.Y && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 1 && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 6 
					 && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 11 && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 12
					 && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 13 && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 14
					 && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 15 && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 16
					 && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 17 && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 18
					 && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 19 && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 20){
		 enemy8.Y += 16;
		 tag = 1;
	 }
	if(tag){
		tag = 0;
		LCD_DrawRGBImage(enemy8.X, enemy8.Y, 16, 16, enemy8.image);  
		LCD_DrawRGBImage(enemy8.pre_X, enemy8.pre_Y, 16, 16, (uint8_t*)0x08012e00); // feces
		occupy[enemy8.X/16][enemy8.Y/16] = 1;				
		occupy[enemy8.pre_X/16][enemy8.pre_Y/16] =  5;		
		enemy8.pre_X = enemy8.X;
		enemy8.pre_Y = enemy8.Y; 
	 }			 
	 
	 if((player.X-16 == enemy9.X && player.Y == enemy9.Y) || (player.X+16 == enemy9.X && player.Y == enemy9.Y)
			||(player.Y-16 == enemy9.Y && player.X == enemy9.X) || (player.Y+16 == enemy9.Y && player.X == enemy9.X) ){
				 player.life -= 1; 	
				 draw_bite(player.X, player.Y);
				 delay_ms(500);
				 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);	
		 }
	 else if(player.X < enemy9.X && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 1 && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 6 
					 && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 11 && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 12
					 && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 13 && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 14
					 && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 15 && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 16
					 && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 17 && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 18
					 && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 19 && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 20){
		 enemy9.X -= 16;
		 tag = 1;
	 }
	 else if(player.Y < enemy9.Y && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 1 && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 6 
					 && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 11 && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 12
					 && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 13 && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 14
					 && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 15 && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 16
					 && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 17 && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 18
					 && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 19 && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 20){
		 enemy9.Y -= 16;
		 tag = 1;
	 }
	 else if(player.X > enemy9.X && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 1 && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 6 
					 && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 11 && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 12
					 && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 13 && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 14
					 && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 15 && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 16
					 && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 17 && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 18
					 && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 19 && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 20){
		 enemy9.X += 16;
		 tag = 1;
	 }
	 else if(player.Y > enemy9.Y && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 1 && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 6 
					 && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 11 && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 12
					 && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 13 && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 14
					 && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 15 && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 16
					 && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 17 && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 18
					 && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 19 && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 20){
		 enemy9.Y += 16;
		 tag = 1;
	 }
	if(tag){
		tag = 0;
		LCD_DrawRGBImage(enemy9.X, enemy9.Y, 16, 16, enemy9.image);  
		LCD_DrawRGBImage(enemy9.pre_X, enemy9.pre_Y, 16, 16, (uint8_t*)0x08012e00); // feces
		occupy[enemy9.X/16][enemy9.Y/16] = 1;				
		occupy[enemy9.pre_X/16][enemy9.pre_Y/16] =  5;		
		enemy9.pre_X = enemy9.X;
		enemy9.pre_Y = enemy9.Y; 
	}			 
	 LCD_SetTextColor(RED);
	 sprintf(S1, "%d ", player.life);
	 LCD_DisplayStringAt(295, 40, S1, LEFT_MODE);		
	}
	return player.life;
}


// #-------------------------#
//        state == 7
// #-------------------------#
// 1: enermy, 2: stone, 3: time, 4: end

void level_6_draw(void) {
	time_stop = 0;
	for(int i = 0; i <= 14 ; i++)
		for(int j = 0; j <= 14 ;j++)
		  LCD_DrawRGBImage(i*16, j*16, 16, 16, (uint8_t*)0x08014000); 
	
	
	player.image = (uint8_t*)0x08014200;		
	player.hide_image = (uint8_t*)0x08014400;
	LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
	
	occupy[14][0] = 4;
	LCD_DrawRGBImage(224, 0, 16, 16, (uint8_t*)0x08013e00); 

	occupy[13][1] = 2; occupy[13][2] = 2; occupy[13][3] = 2; occupy[12][1] = 2; occupy[11][1] = 2; 
	LCD_DrawRGBImage(16*13, 16, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*13, 32, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*13, 48, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*12, 16, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*11, 16, 16, 16, (uint8_t*)0x08013a00);	

	occupy[5][5] = 2; occupy[5][6] = 2; occupy[5][7] = 2; occupy[5][8] = 2; occupy[5][9] = 2;
	occupy[6][5] = 2; occupy[6][6] = 2; occupy[6][7] = 2; occupy[6][8] = 2; occupy[6][9] = 2;
	occupy[7][5] = 2; occupy[7][6] = 2; occupy[7][7] = 2; occupy[7][8] = 2; occupy[7][9] = 2;
	occupy[8][5] = 2; occupy[8][6] = 2; occupy[8][7] = 2; occupy[8][8] = 2; occupy[8][9] = 2;
	occupy[9][5] = 2; occupy[9][6] = 2; occupy[9][7] = 2; occupy[9][8] = 2; occupy[9][9] = 2;		
	LCD_DrawRGBImage(16*5, 16*5, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*5, 16*6, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*5, 16*7, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*5, 16*8, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*5, 16*9, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*6, 16*5, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*6, 16*6, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*6, 16*7, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*6, 16*8, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*6, 16*9, 16, 16, (uint8_t*)0x08013a00);	
	LCD_DrawRGBImage(16*7, 16*5, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*7, 16*6, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*7, 16*7, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*7, 16*8, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*7, 16*9, 16, 16, (uint8_t*)0x08013a00);	
	LCD_DrawRGBImage(16*8, 16*5, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*8, 16*6, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*8, 16*7, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*8, 16*8, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*8, 16*9, 16, 16, (uint8_t*)0x08013a00);	
	LCD_DrawRGBImage(16*9, 16*5, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*9, 16*6, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*9, 16*7, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*9, 16*8, 16, 16, (uint8_t*)0x08013a00);
	LCD_DrawRGBImage(16*9, 16*9, 16, 16, (uint8_t*)0x08013a00);			

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy1.image = (uint8_t*)0x08013800;
	enemy1.X = tem_x * 16;
	enemy1.Y = tem_y * 16;
	enemy1.pre_X = enemy1.X;
	enemy1.pre_Y = enemy1.Y;
	LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, enemy1.image);

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy2.image = (uint8_t*)0x08013800;
	enemy2.X = tem_x * 16;
	enemy2.Y = tem_y * 16;
	enemy2.pre_X = enemy2.X;
	enemy2.pre_Y = enemy2.Y;
	LCD_DrawRGBImage(enemy2.X, enemy2.Y, 16, 16, enemy2.image); 

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy3.image = (uint8_t*)0x08013800;
	enemy3.X = tem_x * 16;
	enemy3.Y = tem_y * 16;
	enemy3.pre_X = enemy3.X;
	enemy3.pre_Y = enemy3.Y;
	LCD_DrawRGBImage(enemy3.X, enemy3.Y, 16, 16, enemy3.image); 

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy4.image = (uint8_t*)0x08013800;
	enemy4.X = tem_x * 16;
	enemy4.Y = tem_y * 16;
	enemy4.pre_X = enemy4.X;
	enemy4.pre_Y = enemy4.Y;
	LCD_DrawRGBImage(enemy4.X, enemy4.Y, 16, 16, enemy4.image); 

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy5.image = (uint8_t*)0x08013800;
	enemy5.X = tem_x * 16;
	enemy5.Y = tem_y * 16;
	enemy5.pre_X = enemy5.X;
	enemy5.pre_Y = enemy5.Y;
	LCD_DrawRGBImage(enemy5.X, enemy5.Y, 16, 16, enemy5.image); 

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy6.image = (uint8_t*)0x08013800;
	enemy6.X = tem_x * 16;
	enemy6.Y = tem_y * 16;
	enemy6.pre_X = enemy6.X;
	enemy6.pre_Y = enemy6.Y;
	LCD_DrawRGBImage(enemy6.X, enemy6.Y, 16, 16, enemy6.image); 

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy7.image = (uint8_t*)0x08013800;
	enemy7.X = tem_x * 16;
	enemy7.Y = tem_y * 16;
	enemy7.pre_X = enemy7.X;
	enemy7.pre_Y = enemy7.Y;
	LCD_DrawRGBImage(enemy7.X, enemy7.Y, 16, 16, enemy7.image); 

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy8.image = (uint8_t*)0x08013800;
	enemy8.X = tem_x * 16;
	enemy8.Y = tem_y * 16;
	enemy8.pre_X = enemy8.X;
	enemy8.pre_Y = enemy8.Y;
	LCD_DrawRGBImage(enemy8.X, enemy8.Y, 16, 16, enemy8.image); 

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy9.image = (uint8_t*)0x08013800;
	enemy9.X = tem_x * 16;
	enemy9.Y = tem_y * 16;
	enemy9.pre_X = enemy9.X;
	enemy9.pre_Y = enemy9.Y;
	LCD_DrawRGBImage(enemy9.X, enemy9.Y, 16, 16, enemy9.image); 

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}
	occupy[tem_x][tem_y] = 1;
	enemy10.image = (uint8_t*)0x08013800;
	enemy10.X = tem_x * 16;
	enemy10.Y = tem_y * 16;
	enemy10.pre_X = enemy10.X;
	enemy10.pre_Y = enemy10.Y;
	LCD_DrawRGBImage(enemy10.X, enemy10.Y, 16, 16, enemy10.image); 		
	
	for(int i = 0; i <= 14; i++){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);
		while(occupy[tem_x][tem_y] != 0){
			tem_x = RNG_Get_RandomRange(1,13);
			tem_y = RNG_Get_RandomRange(1,13);		    
		}		
		occupy[tem_x][tem_y] = 2;
		LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013a00);
	}

	for(int i = 0; i <= 19; i++){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);
		while(occupy[tem_x][tem_y] != 0){
			tem_x = RNG_Get_RandomRange(1,13);
			tem_y = RNG_Get_RandomRange(1,13);		    
		}		
		occupy[tem_x][tem_y] = 3;
		LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013c00);
	}
}

int level_6_flow(void) {
	tag = 0;
	TSC_TouchDet(1);  
	TS_GetState(&Touch);
	touch_x = -1;
	touch_y = -1;
	if (Touch.TouchDetected){
		touch_x = Touch.x;
		touch_y = Touch.y;
		WaitForTouchRelease(6);
	}

  if(touch_x >= 245 && touch_x <= 309 && touch_y >= 85 && touch_y <= 149 && player.skill == 2){
	  LCD_DrawRGBImage( 245, 85, 64, 64, player.skill_image);
	  player.skill = 1;
	  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);  
  }
  else if(touch_x >= btnUp.X && touch_x <= btnUp.X + 15 && touch_y >= btnUp.Y && touch_y <= btnUp.Y + 25 && player.Y != 0 
				 && occupy[player.X/16][(player.Y-16)/16]!=1 && occupy[player.X/16][(player.Y-16)/16] != 2){  // up
		player.Y -= 16;
		tag = 1;
	}
  else if(touch_x >= btnDown.X && touch_x <= btnDown.X + 15 && touch_y >= btnDown.Y && touch_y <= btnDown.Y + 25 && player.Y != 224 
				 && occupy[player.X/16][(player.Y+16)/16]!=1 && occupy[player.X/16][(player.Y+16)/16] != 2){ // down
		player.Y += 16;
		tag = 1;
  }
  else if(touch_x >= btnLeft.X && touch_x <= btnLeft.X + 25 && touch_y >= btnLeft.Y && touch_y <= btnLeft.Y + 15 && player.X != 0 
				 && occupy[(player.X-16)/16][player.Y/16]!=1 && occupy[(player.X-16)/16][player.Y/16] != 2){ //left
		player.X -= 16;
		tag = 1;
  } 
  else if(touch_x >= btnRight.X && touch_x <= btnRight.X + 25 && touch_y >= btnRight.Y && touch_y <= btnRight.Y + 15 && player.X != 224 
				 && occupy[(player.X+16)/16][player.Y/16]!=1 && occupy[(player.X+16)/16][player.Y/16] != 2){ // right
		player.X += 16;
		tag = 1;
  }
 
  if(tag){
		if(time_stop > 0)
			time_stop--;
		
		if(player.skill == 0){
			player.skill_round++;
			if(player.skill_round == 4){
				player.skill = 2;
				LCD_DrawRGBImage( 245, 85, 64, 64, player.unskill_image);
			}
		}
		else if(player.skill == 1){
			player.skill_round--;
			if(player.skill_round == 0){
				player.skill = 0;
				LCD_SetTextColor(BLACK);
				LCD_FillRect(245, 85, 64, 64);
				LCD_SetTextColor(WHITE);
			}
		}
		
		if(player.skill != 1)
			 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);  
		else
			 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
		LCD_DrawRGBImage(player.pre_X, player.pre_Y, 16, 16, (uint8_t*)0x08014000); //reproduction backgroud
		player.pre_X = player.X;
		player.pre_Y = player.Y;
 }
 
 if(tag && occupy[player.X/16][player.Y/16] > 2){
		if(occupy[player.X/16][player.Y/16] == 3){
			time_stop += 2;
			occupy[player.X/16][player.Y/16] = 0;
		}
		else if(occupy[player.X/16][player.Y/16] == 4){
			LCD_SetTextColor(YELLOW);
			LCD_SetFont(&Font16);
			LCD_DisplayStringAt(0, 60, "Chameleon escaped all levels", CENTER_MODE);		
			LCD_DisplayStringAt(0, 90, "^o^   Congratulations  ^o^", CENTER_MODE);
			LCD_DisplayStringAt(0, 160, "   Click to quit   ", CENTER_MODE);					
			return 1000;
		}	
 }
	 
  if(tag && player.skill != 1 && time_stop == 0){
	  tag = 0;	 
	
	  if((player.X-16 == enemy1.X && player.Y == enemy1.Y) || (player.X+16 == enemy1.X && player.Y == enemy1.Y)
			||(player.Y-16 == enemy1.Y && player.X == enemy1.X) || (player.Y+16 == enemy1.Y && player.X == enemy1.X) ){
		  draw_splatter(player.X, player.Y);
		  delay_ms(1000);
		  return 0;
	  }
	  else if(player.X < enemy1.X && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 1 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 2 
					 && occupy[(enemy1.X - 16)/16][enemy1.Y/16] != 4){
		  enemy1.X -= 16;
		  tag = 1;
	  }
	  else if(player.Y < enemy1.Y && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 1 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 2 
					 && occupy[enemy1.X/16][(enemy1.Y - 16)/16] != 4){
		  enemy1.Y -= 16;
		  tag = 1;
	  }
	  else if(player.X > enemy1.X && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 1 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 2 
					 && occupy[(enemy1.X + 16)/16][enemy1.Y/16] != 4){
		  enemy1.X += 16;
		  tag = 1;
	  }
	  else if(player.Y > enemy1.Y && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 1 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 2 
					 && occupy[enemy1.X/16][(enemy1.Y + 16)/16] != 4){
		  enemy1.Y += 16;
		  tag = 1;
	  }	 
	  if(tag){
		  tag = 0;
		  LCD_DrawRGBImage(enemy1.X, enemy1.Y, 16, 16, enemy1.image);  
		  LCD_DrawRGBImage(enemy1.pre_X, enemy1.pre_Y, 16, 16, (uint8_t*)0x08014000); // reproduction backgroud
		  occupy[enemy1.X/16][enemy1.Y/16] = 1;				
		  occupy[enemy1.pre_X/16][enemy1.pre_Y/16] = 0;		
		  enemy1.pre_X = enemy1.X;
		  enemy1.pre_Y = enemy1.Y; 
	  }	

	  if((player.X-16 == enemy2.X && player.Y == enemy2.Y) || (player.X+16 == enemy2.X && player.Y == enemy2.Y)
			 ||(player.Y-16 == enemy2.Y && player.X == enemy2.X) || (player.Y+16 == enemy2.Y && player.X == enemy2.X) ){
	    draw_splatter(player.X, player.Y);
	    delay_ms(1000);	
	    return 0;
	  }
	  else if(player.X < enemy2.X && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 1 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 2 
					 && occupy[(enemy2.X - 16)/16][enemy2.Y/16] != 4){
		  enemy2.X -= 16;
		  tag = 1;
	  }
	  else if(player.Y < enemy2.Y && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 1 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 2 
					 && occupy[enemy2.X/16][(enemy2.Y - 16)/16] != 4){
		  enemy2.Y -= 16;
		  tag = 1;
	  }
	  else if(player.X > enemy2.X && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 1 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 2 
					 && occupy[(enemy2.X + 16)/16][enemy2.Y/16] != 4){
		  enemy2.X += 16;
		  tag = 1;
	  }
	  else if(player.Y > enemy2.Y && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 1 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 2 
					 && occupy[enemy2.X/16][(enemy2.Y + 16)/16] != 4){
		  enemy2.Y += 16;
		  tag = 1;
	  }	 
	  if(tag){
		  tag = 0;
		  LCD_DrawRGBImage(enemy2.X, enemy2.Y, 16, 16, enemy2.image);  
		  LCD_DrawRGBImage(enemy2.pre_X, enemy2.pre_Y, 16, 16, (uint8_t*)0x08014000); // reproduction backgroud
		  occupy[enemy2.X/16][enemy2.Y/16] = 1;				
		  occupy[enemy2.pre_X/16][enemy2.pre_Y/16] = 0;		
		  enemy2.pre_X = enemy2.X;
		  enemy2.pre_Y = enemy2.Y; 
	  }			

	  if((player.X-16 == enemy3.X && player.Y == enemy3.Y) || (player.X+16 == enemy3.X && player.Y == enemy3.Y)
			 ||(player.Y-16 == enemy3.Y && player.X == enemy3.X) || (player.Y+16 == enemy3.Y && player.X == enemy3.X) ){
		  draw_splatter(player.X, player.Y);
		  delay_ms(1000);
		  return 0;
	  }
	  else if(player.X < enemy3.X && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 1 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 2 
					 && occupy[(enemy3.X - 16)/16][enemy3.Y/16] != 4){
		  enemy3.X -= 16;
		  tag = 1;
	  }
	  else if(player.Y < enemy3.Y && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 1 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 2 
					 && occupy[enemy3.X/16][(enemy3.Y - 16)/16] != 4){
		  enemy3.Y -= 16;
		  tag = 1;
	  }
	  else if(player.X > enemy3.X && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 1 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 2 
					 && occupy[(enemy3.X + 16)/16][enemy3.Y/16] != 4){
		  enemy3.X += 16;
		  tag = 1;
	  }
	  else if(player.Y > enemy3.Y && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 1 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 2 
					 && occupy[enemy3.X/16][(enemy3.Y + 16)/16] != 4){
		  enemy3.Y += 16;
		  tag = 1;
	  }	 
	  if(tag){
		  tag = 0;
		  LCD_DrawRGBImage(enemy3.X, enemy3.Y, 16, 16, enemy3.image);  
		  LCD_DrawRGBImage(enemy3.pre_X, enemy3.pre_Y, 16, 16, (uint8_t*)0x08014000); // reproduction backgroud
		  occupy[enemy3.X/16][enemy3.Y/16] = 1;				
		  occupy[enemy3.pre_X/16][enemy3.pre_Y/16] = 0;		
		  enemy3.pre_X = enemy3.X;
		  enemy3.pre_Y = enemy3.Y; 
	  } 			

	  if((player.X-16 == enemy4.X && player.Y == enemy4.Y) || (player.X+16 == enemy4.X && player.Y == enemy4.Y)
			  ||(player.Y-16 == enemy4.Y && player.X == enemy4.X) || (player.Y+16 == enemy4.Y && player.X == enemy4.X) ){
	    draw_splatter(player.X, player.Y);
	    delay_ms(1000);	
	    return 0;
	  }	
	  else if(player.X < enemy4.X && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 1 && occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 2 
			 	  	&& occupy[(enemy4.X - 16)/16][enemy4.Y/16] != 4){
		  enemy4.X -= 16;
		  tag = 1;
 	  }
	  else if(player.Y < enemy4.Y && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 1 && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 2 
					  && occupy[enemy4.X/16][(enemy4.Y - 16)/16] != 4){
		  enemy4.Y -= 16;
	  	tag = 1;
	  }
	  else if(player.X > enemy4.X && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 1 && occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 2 
				  	&& occupy[(enemy4.X + 16)/16][enemy4.Y/16] != 4){
		  enemy4.X += 16;
		  tag = 1;
 	  }
	  else if(player.Y > enemy4.Y && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 1 && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 2 
					  && occupy[enemy4.X/16][(enemy4.Y + 16)/16] != 4){
		  enemy4.Y += 16;
		  tag = 1;
	  }	 
	  if(tag){
		  tag = 0;
		  LCD_DrawRGBImage(enemy4.X, enemy4.Y, 16, 16, enemy4.image);  
		  LCD_DrawRGBImage(enemy4.pre_X, enemy4.pre_Y, 16, 16, (uint8_t*)0x08014000); // reproduction backgroud
		  occupy[enemy4.X/16][enemy4.Y/16] = 1;				
		  occupy[enemy4.pre_X/16][enemy4.pre_Y/16] = 0;		
		  enemy4.pre_X = enemy4.X;
		  enemy4.pre_Y = enemy4.Y; 
	  }			

	  if((player.X-16 == enemy5.X && player.Y == enemy5.Y) || (player.X+16 == enemy5.X && player.Y == enemy5.Y)
			  ||(player.Y-16 == enemy5.Y && player.X == enemy5.X) || (player.Y+16 == enemy5.Y && player.X == enemy5.X) ){
		  draw_splatter(player.X, player.Y);
		  delay_ms(1000);
		  return 0;
	  }
	  else if(player.X < enemy5.X && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 1 && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 2 
					  && occupy[(enemy5.X - 16)/16][enemy5.Y/16] != 4){
		  enemy5.X -= 16;
		  tag = 1;
	  }
	  else if(player.Y < enemy5.Y && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 1 && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 2 
					  && occupy[enemy5.X/16][(enemy5.Y - 16)/16] != 4){
		  enemy5.Y -= 16;
		  tag = 1;
	  }
	  else if(player.X > enemy5.X && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 1 && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 2 
					  && occupy[(enemy5.X + 16)/16][enemy5.Y/16] != 4){
		  enemy5.X += 16;
		  tag = 1;
	  }
	  else if(player.Y > enemy5.Y && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 1 && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 2 
					  && occupy[enemy5.X/16][(enemy5.Y + 16)/16] != 4){
		  enemy5.Y += 16;
		  tag = 1;
	  }	 
	  if(tag){
		  tag = 0;
		  LCD_DrawRGBImage(enemy5.X, enemy5.Y, 16, 16, enemy5.image);  
		  LCD_DrawRGBImage(enemy5.pre_X, enemy5.pre_Y, 16, 16, (uint8_t*)0x08014000); // reproduction backgroud
		  occupy[enemy5.X/16][enemy5.Y/16] = 1;				
		  occupy[enemy5.pre_X/16][enemy5.pre_Y/16] = 0;		
		  enemy5.pre_X = enemy5.X;
		  enemy5.pre_Y = enemy5.Y; 
	  }			

	  if((player.X-16 == enemy6.X && player.Y == enemy6.Y) || (player.X+16 == enemy6.X && player.Y == enemy6.Y)
		  	||(player.Y-16 == enemy6.Y && player.X == enemy6.X) || (player.Y+16 == enemy6.Y && player.X == enemy6.X) ){
	    draw_splatter(player.X, player.Y);
	    delay_ms(1000);
	    return 0;
	  }
	  else if(player.X < enemy6.X && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 1 && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 2 
					  && occupy[(enemy6.X - 16)/16][enemy6.Y/16] != 4){
		  enemy6.X -= 16;
		  tag = 1;
	  }
	  else if(player.Y < enemy6.Y && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 1 && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 2 
					  && occupy[enemy6.X/16][(enemy6.Y - 16)/16] != 4){
		  enemy6.Y -= 16;
		  tag = 1;
	  }
	  else if(player.X > enemy6.X && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 1 && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 2 
					  && occupy[(enemy6.X + 16)/16][enemy6.Y/16] != 4){
		  enemy6.X += 16;
		  tag = 1;
	  }
	  else if(player.Y > enemy6.Y && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 1 && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 2 
					  && occupy[enemy6.X/16][(enemy6.Y + 16)/16] != 4){
		  enemy6.Y += 16;
		  tag = 1;
	  }	 
	  if(tag){
		  tag = 0;
		  LCD_DrawRGBImage(enemy6.X, enemy6.Y, 16, 16, enemy6.image);  
		  LCD_DrawRGBImage(enemy6.pre_X, enemy6.pre_Y, 16, 16, (uint8_t*)0x08014000); // reproduction backgroud
		  occupy[enemy6.X/16][enemy6.Y/16] = 1;				
		  occupy[enemy6.pre_X/16][enemy6.pre_Y/16] = 0;		
		  enemy6.pre_X = enemy6.X;
		  enemy6.pre_Y = enemy6.Y; 
	  }			

	  if((player.X-16 == enemy7.X && player.Y == enemy7.Y) || (player.X+16 == enemy7.X && player.Y == enemy7.Y)
			  ||(player.Y-16 == enemy7.Y && player.X == enemy7.X) || (player.Y+16 == enemy7.Y && player.X == enemy7.X) ){
		  draw_splatter(player.X, player.Y);
		  delay_ms(1000);
		  return 0;
	  }
	  else if(player.X < enemy7.X && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 1 && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 2 
					  && occupy[(enemy7.X - 16)/16][enemy7.Y/16] != 4){
		  enemy7.X -= 16;
		  tag = 1;
	  }
	  else if(player.Y < enemy7.Y && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 1 && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 2 
					  && occupy[enemy7.X/16][(enemy7.Y - 16)/16] != 4){
		  enemy7.Y -= 16;
		  tag = 1;
	  }
	  else if(player.X > enemy7.X && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 1 && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 2 
					  && occupy[(enemy7.X + 16)/16][enemy7.Y/16] != 4){
		  enemy7.X += 16;
		  tag = 1;
	  }
	  else if(player.Y > enemy7.Y && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 1 && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 2 
					  && occupy[enemy7.X/16][(enemy7.Y + 16)/16] != 4){
		  enemy7.Y += 16;
		  tag = 1;
	  }	 
	  if(tag){
		  tag = 0;
		  LCD_DrawRGBImage(enemy7.X, enemy7.Y, 16, 16, enemy7.image);  
		  LCD_DrawRGBImage(enemy7.pre_X, enemy7.pre_Y, 16, 16, (uint8_t*)0x08014000); // reproduction backgroud
		  occupy[enemy7.X/16][enemy7.Y/16] = 1;				
		  occupy[enemy7.pre_X/16][enemy7.pre_Y/16] = 0;		
		  enemy7.pre_X = enemy7.X;
		  enemy7.pre_Y = enemy7.Y; 
	  }			

	  if((player.X-16 == enemy8.X && player.Y == enemy8.Y) || (player.X+16 == enemy8.X && player.Y == enemy8.Y)
			  ||(player.Y-16 == enemy8.Y && player.X == enemy8.X) || (player.Y+16 == enemy8.Y && player.X == enemy8.X) ){
		  draw_splatter(player.X, player.Y);
		  delay_ms(1000);
		  return 0;
	  }
	  else if(player.X < enemy8.X && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 1 && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 2 
					  && occupy[(enemy8.X - 16)/16][enemy8.Y/16] != 4){
		  enemy8.X -= 16;
		  tag = 1;
	  }
	  else if(player.Y < enemy8.Y && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 1 && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 2 
					  && occupy[enemy8.X/16][(enemy8.Y - 16)/16] != 4){
		  enemy8.Y -= 16;
		  tag = 1;
	  }
	  else if(player.X > enemy8.X && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 1 && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 2 
					  && occupy[(enemy8.X + 16)/16][enemy8.Y/16] != 4){
		  enemy8.X += 16;
		  tag = 1;
	  }
	  else if(player.Y > enemy8.Y && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 1 && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 2 
					  && occupy[enemy8.X/16][(enemy8.Y + 16)/16] != 4){
		  enemy8.Y += 16;
	    tag = 1;
	  }	 
	  if(tag){
		  tag = 0;
		  LCD_DrawRGBImage(enemy8.X, enemy8.Y, 16, 16, enemy8.image);  
		  LCD_DrawRGBImage(enemy8.pre_X, enemy8.pre_Y, 16, 16, (uint8_t*)0x08014000); // reproduction backgroud
		  occupy[enemy8.X/16][enemy8.Y/16] = 1;				
		  occupy[enemy8.pre_X/16][enemy8.pre_Y/16] = 0;		
		  enemy8.pre_X = enemy8.X;
		  enemy8.pre_Y = enemy8.Y; 
	  }			

	  if((player.X-16 == enemy9.X && player.Y == enemy9.Y) || (player.X+16 == enemy9.X && player.Y == enemy9.Y)
			  ||(player.Y-16 == enemy9.Y && player.X == enemy9.X) || (player.Y+16 == enemy9.Y && player.X == enemy9.X) ){
	    draw_splatter(player.X, player.Y);
		  delay_ms(1000);	
		  return 0;
 	  }
	  else if(player.X < enemy9.X && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 1 && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 2 
					  && occupy[(enemy9.X - 16)/16][enemy9.Y/16] != 4){
		  enemy9.X -= 16;
		  tag = 1;
	  }
	  else if(player.Y < enemy9.Y && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 1 && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 2 
					  && occupy[enemy9.X/16][(enemy9.Y - 16)/16] != 4){
		  enemy9.Y -= 16;
		  tag = 1;
	  }
	  else if(player.X > enemy9.X && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 1 && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 2 
					  && occupy[(enemy9.X + 16)/16][enemy9.Y/16] != 4){
		  enemy9.X += 16;
		  tag = 1;
	  }
	  else if(player.Y > enemy9.Y && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 1 && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 2 
					  && occupy[enemy9.X/16][(enemy9.Y + 16)/16] != 4){
		  enemy9.Y += 16;
		  tag = 1;
	  }	 
	  if(tag){
		  tag = 0;
		  LCD_DrawRGBImage(enemy9.X, enemy9.Y, 16, 16, enemy9.image);  
		  LCD_DrawRGBImage(enemy9.pre_X, enemy9.pre_Y, 16, 16, (uint8_t*)0x08014000); // reproduction backgroud
		  occupy[enemy9.X/16][enemy9.Y/16] = 1;				
		  occupy[enemy9.pre_X/16][enemy9.pre_Y/16] = 0;		
		  enemy9.pre_X = enemy9.X;
		  enemy9.pre_Y = enemy9.Y; 
	  }			

	  if((player.X-16 == enemy10.X && player.Y == enemy10.Y) || (player.X+16 == enemy10.X && player.Y == enemy10.Y)
			  ||(player.Y-16 == enemy10.Y && player.X == enemy10.X) || (player.Y+16 == enemy10.Y && player.X == enemy10.X) ){
		  draw_splatter(player.X, player.Y);
		  delay_ms(1000);
		  return 0;
	  }
	  else if(player.X < enemy10.X && occupy[(enemy10.X - 16)/16][enemy10.Y/16] != 1 && occupy[(enemy10.X - 16)/16][enemy10.Y/16] != 2 
					  && occupy[(enemy10.X - 16)/16][enemy10.Y/16] != 4){
		  enemy10.X -= 16;
		  tag = 1;
	  }
	  else if(player.Y < enemy10.Y && occupy[enemy10.X/16][(enemy10.Y - 16)/16] != 1 && occupy[enemy10.X/16][(enemy10.Y - 16)/16] != 2 
					  && occupy[enemy10.X/16][(enemy10.Y - 16)/16] != 4){
		  enemy10.Y -= 16;
		  tag = 1;
	  }
	  else if(player.X > enemy10.X && occupy[(enemy10.X + 16)/16][enemy10.Y/16] != 1 && occupy[(enemy10.X + 16)/16][enemy10.Y/16] != 2 
					  && occupy[(enemy10.X + 16)/16][enemy10.Y/16] != 4){
		  enemy10.X += 16;
		  tag = 1;
	  }
	  else if(player.Y > enemy10.Y && occupy[enemy10.X/16][(enemy10.Y + 16)/16] != 1 && occupy[enemy10.X/16][(enemy10.Y + 16)/16] != 2 
					  && occupy[enemy10.X/16][(enemy10.Y + 16)/16] != 4){
		  enemy10.Y += 16;
		  tag = 1;
	  }	 
	  if(tag){
		  tag = 0;
		  LCD_DrawRGBImage(enemy10.X, enemy10.Y, 16, 16, enemy10.image);  
		  LCD_DrawRGBImage(enemy10.pre_X, enemy10.pre_Y, 16, 16, (uint8_t*)0x08014000); // reproduction backgroud
		  occupy[enemy10.X/16][enemy10.Y/16] = 1;				
		  occupy[enemy10.pre_X/16][enemy10.pre_Y/16] = 0;		
		  enemy10.pre_X = enemy10.X;
		  enemy10.pre_Y = enemy10.Y; 
	  }						 
	  LCD_SetTextColor(RED);
	  sprintf(S1, "%d ", player.life);
	  LCD_DisplayStringAt(295, 40, S1, LEFT_MODE);		
  }

  if((player.X-16 == enemy1.X && player.Y == enemy1.Y) || (player.X+16 == enemy1.X && player.Y == enemy1.Y)
	    ||(player.Y-16 == enemy1.Y && player.X == enemy1.X) || (player.Y+16 == enemy1.Y && player.X == enemy1.X) ){
	  draw_splatter(player.X, player.Y);
	  delay_ms(1000);
	  return 0;
	 } 
  if((player.X-16 == enemy2.X && player.Y == enemy2.Y) || (player.X+16 == enemy2.X && player.Y == enemy2.Y)
	    ||(player.Y-16 == enemy2.Y && player.X == enemy2.X) || (player.Y+16 == enemy2.Y && player.X == enemy2.X) ){
		 draw_splatter(player.X, player.Y);
		 delay_ms(1000);	
		 return 0;
	} 
  if((player.X-16 == enemy3.X && player.Y == enemy3.Y) || (player.X+16 == enemy3.X && player.Y == enemy3.Y)
	    ||(player.Y-16 == enemy3.Y && player.X == enemy3.X) || (player.Y+16 == enemy3.Y && player.X == enemy3.X) ){
		 draw_splatter(player.X, player.Y);
		 delay_ms(1000);
		 return 0;
	}  
  if((player.X-16 == enemy4.X && player.Y == enemy4.Y) || (player.X+16 == enemy4.X && player.Y == enemy4.Y)
	    ||(player.Y-16 == enemy4.Y && player.X == enemy4.X) || (player.Y+16 == enemy4.Y && player.X == enemy4.X) ){
		 draw_splatter(player.X, player.Y);
		 delay_ms(1000);
		 return 0;
	 } 
  if((player.X-16 == enemy5.X && player.Y == enemy5.Y) || (player.X+16 == enemy5.X && player.Y == enemy5.Y)
	   ||(player.Y-16 == enemy5.Y && player.X == enemy5.X) || (player.Y+16 == enemy5.Y && player.X == enemy5.X) ){
		 draw_splatter(player.X, player.Y);
		 delay_ms(1000);
		 return 0;
	} 
  if((player.X-16 == enemy6.X && player.Y == enemy6.Y) || (player.X+16 == enemy6.X && player.Y == enemy6.Y)
	    ||(player.Y-16 == enemy6.Y && player.X == enemy6.X) || (player.Y+16 == enemy6.Y && player.X == enemy6.X) ){
		 draw_splatter(player.X, player.Y);
		 delay_ms(1000);	
		 return 0;
	} 
  if((player.X-16 == enemy7.X && player.Y == enemy7.Y) || (player.X+16 == enemy7.X && player.Y == enemy7.Y)
	    ||(player.Y-16 == enemy7.Y && player.X == enemy7.X) || (player.Y+16 == enemy7.Y && player.X == enemy7.X) ){
		 draw_splatter(player.X, player.Y);
		 delay_ms(1000);
		 return 0;
	} 
  if((player.X-16 == enemy8.X && player.Y == enemy8.Y) || (player.X+16 == enemy8.X && player.Y == enemy8.Y)
	    ||(player.Y-16 == enemy8.Y && player.X == enemy8.X) || (player.Y+16 == enemy8.Y && player.X == enemy8.X) ){
		 draw_splatter(player.X, player.Y);
		 delay_ms(1000);
		 return 0;
	 } 
  if((player.X-16 == enemy9.X && player.Y == enemy9.Y) || (player.X+16 == enemy9.X && player.Y == enemy9.Y)
	    ||(player.Y-16 == enemy9.Y && player.X == enemy9.X) || (player.Y+16 == enemy9.Y && player.X == enemy9.X) ){
		 draw_splatter(player.X, player.Y);
		 delay_ms(1000);
		 return 0;
	}  
  if((player.X-16 == enemy10.X && player.Y == enemy10.Y) || (player.X+16 == enemy10.X && player.Y == enemy10.Y)
	    ||(player.Y-16 == enemy10.Y && player.X == enemy10.X) || (player.Y+16 == enemy10.Y && player.X == enemy10.X) ){
		 draw_splatter(player.X, player.Y);
		 delay_ms(1000);
		 return 0;
	} 
 return player.life;
}

// #-------------------------#
//        state == 7
// #-------------------------#
// 1: end, 11~40 : portol
void hidden_draw(void) {
	portal_tag = 0;
	for(int i = 0; i <= 14 ; i++)
		for(int j = 0; j <= 14 ;j++)
			LCD_DrawRGBImage(i*16, j*16, 16, 16, (uint8_t*)0x08010e00); 
	
	player.image = (uint8_t*)0x08010c00;		
	player.hide_image = (uint8_t*)0x08011000;
	LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
	player.life+=12;
	LCD_SetTextColor(RED);
	sprintf(S1, "%d ", player.life);
	LCD_DisplayStringAt(295, 40, S1, LEFT_MODE);	

	occupy[14][0] = 1;
	LCD_DrawRGBImage(224, 0, 16, 16, (uint8_t*)0x08012200); 	

	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 11;
	portal_1.X = tem_x * 16;
	portal_1.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);		
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 12;
	portal_2.X = tem_x * 16;
	portal_2.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 13;
	portal_3.X = tem_x * 16;
	portal_3.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 14;
	portal_4.X = tem_x * 16;
	portal_4.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 15;
	portal_5.X = tem_x * 16;
	portal_5.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 16;
	portal_6.X = tem_x * 16;
	portal_6.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 17;
	portal_7.X = tem_x * 16;
	portal_7.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 18;
	portal_8.X = tem_x * 16;
	portal_8.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 19;
	portal_9.X = tem_x * 16;
	portal_9.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 20;
	portal_10.X = tem_x * 16;
	portal_10.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 21;
	portal_11.X = tem_x * 16;
	portal_11.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);		
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 22;
	portal_12.X = tem_x * 16;
	portal_12.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 23;
	portal_13.X = tem_x * 16;
	portal_13.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 24;
	portal_14.X = tem_x * 16;
	portal_14.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 25;
	portal_15.X = tem_x * 16;
	portal_15.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 26;
	portal_16.X = tem_x * 16;
	portal_16.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 27;
	portal_17.X = tem_x * 16;
	portal_17.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 28;
	portal_18.X = tem_x * 16;
	portal_18.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 29;
	portal_19.X = tem_x * 16;
	portal_19.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 30;
	portal_20.X = tem_x * 16;
	portal_20.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 31;
	portal_21.X = tem_x * 16;
	portal_21.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);		
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 32;
	portal_22.X = tem_x * 16;
	portal_22.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 33;
	portal_23.X = tem_x * 16;
	portal_23.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 34;
	portal_24.X = tem_x * 16;
	portal_24.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 35;
	portal_25.X = tem_x * 16;
	portal_25.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 36;
	portal_26.X = tem_x * 16;
	portal_26.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 37;
	portal_27.X = tem_x * 16;
	portal_27.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 38;
	portal_28.X = tem_x * 16;
	portal_28.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 39;
	portal_29.X = tem_x * 16;
	portal_29.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
	
	tem_x = RNG_Get_RandomRange(1,13);
	tem_y = RNG_Get_RandomRange(1,13);
	while(occupy[tem_x][tem_y] != 0){
		tem_x = RNG_Get_RandomRange(1,13);
		tem_y = RNG_Get_RandomRange(1,13);		    
	}		
	occupy[tem_x][tem_y] = 40;
	portal_30.X = tem_x * 16;
	portal_30.Y = tem_y * 16;
	LCD_DrawRGBImage(tem_x * 16, tem_y * 16, 16, 16, (uint8_t*)0x08013200);
}

int hidden_flow(void) {
	tag = 0;
	TSC_TouchDet(1);  
	TS_GetState(&Touch);
	touch_x = -1;
	touch_y = -1;
	if (Touch.TouchDetected){
		touch_x = Touch.x;
		touch_y = Touch.y;
		WaitForTouchRelease(6);
	}

  if(touch_x >= 245 && touch_x <= 309 && touch_y >= 85 && touch_y <= 149 && player.skill == 2){
	  LCD_DrawRGBImage( 245, 85, 64, 64, player.skill_image);
	  player.skill = 1;
	  LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);  
  }
  else if(touch_x >= btnUp.X && touch_x <= btnUp.X + 15 && touch_y >= btnUp.Y && touch_y <= btnUp.Y + 25 && player.Y != 0){  // up
		player.Y -= 16;
		tag = 1;
	}
  else if(touch_x >= btnDown.X && touch_x <= btnDown.X + 15 && touch_y >= btnDown.Y && touch_y <= btnDown.Y + 25 && player.Y != 224){ // down
		player.Y += 16;
		tag = 1;
  }
  else if(touch_x >= btnLeft.X && touch_x <= btnLeft.X + 25 && touch_y >= btnLeft.Y && touch_y <= btnLeft.Y + 15 && player.X != 0){ //left
		player.X -= 16;
		tag = 1;
  } 
  else if(touch_x >= btnRight.X && touch_x <= btnRight.X + 25 && touch_y >= btnRight.Y && touch_y <= btnRight.Y + 15 && player.X != 224){ // right
		player.X += 16;
		tag = 1;
  }
 
  if(tag){
		if(player.skill == 0){
			player.skill_round++;
			if(player.skill_round == 4){
				player.skill = 2;
				LCD_DrawRGBImage( 245, 85, 64, 64, player.unskill_image);
			}
		}
		else if(player.skill == 1){
			player.skill_round--;
			if(player.skill_round == 0){
				player.skill = 0;
				LCD_SetTextColor(BLACK);
				LCD_FillRect(245, 85, 64, 64);
				LCD_SetTextColor(WHITE);
			}
		}
		
		if(player.skill != 1)
			 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image);  
		else
			 LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
		if(portal_tag == 1){
			 LCD_DrawRGBImage(player.pre_X, player.pre_Y, 16, 16, (uint8_t*)0x08013200);  // portal
			 portal_tag = 0;
		}
		else
			 LCD_DrawRGBImage(player.pre_X, player.pre_Y, 16, 16, (uint8_t*)0x08010e00); // reproduction backgroud
		player.pre_X = player.X;
		player.pre_Y = player.Y;
		
		player.life--;
		LCD_SetTextColor(RED);
		sprintf(S1, "%d ", player.life);
		LCD_DisplayStringAt(295, 40, S1, LEFT_MODE);	
 }
 
  if(tag && occupy[player.X/16][player.Y/16] >= 1){
	  if(occupy[player.X/16][player.Y/16] == 11){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_2.X;
			player.Y = portal_2.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 12){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_1.X;
			player.Y = portal_1.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 13){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_4.X;
			player.Y = portal_4.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 14){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_3.X;
			player.Y = portal_3.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 15){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_6.X;
			player.Y = portal_6.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 16){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_5.X;
			player.Y = portal_5.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 17){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_8.X;
			player.Y = portal_8.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 18){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_7.X;
			player.Y = portal_7.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 19){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_10.X;
			player.Y = portal_10.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 20){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_9.X;
			player.Y = portal_9.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 21){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_12.X;
			player.Y = portal_12.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 22){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_11.X;
			player.Y = portal_11.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 23){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_14.X;
			player.Y = portal_14.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 24){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_13.X;
			player.Y = portal_13.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 25){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_16.X;
			player.Y = portal_16.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 26){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_15.X;
			player.Y = portal_15.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 27){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_18.X;
			player.Y = portal_18.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 28){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_17.X;
			player.Y = portal_17.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 29){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_20.X;
			player.Y = portal_20.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 30){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_19.X;
			player.Y = portal_19.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 31){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_22.X;
			player.Y = portal_22.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 32){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_21.X;
			player.Y = portal_21.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 33){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_24.X;
			player.Y = portal_24.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 34){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_23.X;
			player.Y = portal_23.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 35){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_26.X;
			player.Y = portal_26.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 36){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_25.X;
			player.Y = portal_25.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 37){
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_28.X;
			player.Y = portal_28.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 38){					
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_27.X;
			player.Y = portal_27.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 39){						
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_30.X;
			player.Y = portal_30.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 40){					
			portal_tag = 1;
			LCD_DrawRGBImage(player.X, player.Y, 16, 16, (uint8_t*)0x08013200); // portal
			player.X = portal_29.X;
			player.Y = portal_29.Y;
			if(player.skill != 1)
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.image); 
			else 
				LCD_DrawRGBImage(player.X, player.Y, 16, 16, player.hide_image);
			player.pre_X = player.X;
			player.pre_Y = player.Y;
		}
		else if(occupy[player.X/16][player.Y/16] == 1){
			LCD_SetTextColor(YELLOW);
			LCD_SetFont(&Font16);
			LCD_DisplayStringAt(0, 60, "!! Escaped hidden level !!", CENTER_MODE);		
			LCD_DisplayStringAt(0, 90, "^o^   Congratulations  ^o^", CENTER_MODE);
			LCD_DisplayStringAt(0, 160, "   Click to quit   ", CENTER_MODE);					
			return 1000;
		}			
  }
	return player.life;
}



// #-------------------------#
//        state == 15
// #-------------------------#
uint8_t game_over(void){
	TSC_TouchDet(1);  
	TS_GetState(&Touch);
	touch_x = -1;
	touch_y = -1;
	if (Touch.TouchDetected){
		touch_x = Touch.x;
		touch_y = Touch.y;
		WaitForTouchRelease(6);
	}
	if(touch_x >= 0 && touch_x <= 320 && touch_y >= 0 && touch_y <= 240)
		return 0;
	else 
		 return 15;
}

// #-------------------------#
//        state == 16
// #-------------------------#
uint8_t judge_next_level(state_tem) {
	TSC_TouchDet(1);  
	TS_GetState(&Touch);
	touch_x = -1;
	touch_y = -1;
	if (Touch.TouchDetected){
		touch_x = Touch.x;
		touch_y = Touch.y;
		WaitForTouchRelease(6);
	}
	
	if(state_tem == 7){
		if(touch_x >= 65 && touch_x <= 255 && touch_y >= 160 && touch_y <= 180)
			return 0;
		else 
			return 16;
	}
	else{
		if(touch_x >= 65 && touch_x <= 255 && touch_y >= 130 && touch_y <= 150)
			return state_tem + 1;
		else if(touch_x >= 65 && touch_x <= 255 && touch_y >= 160 && touch_y <= 180)
			return 0;
		else 
			 return 16;
	}
}

// #-------------------------#
//        state == 17
// #-------------------------#
void level_selector_screen(){
	LCD_SetTextColor(GRAY);
	LCD_FillRect(0, 0, 320, 240);
	LCD_SetTextColor(RED);
	LCD_FillRect(290, 202, 30, 38);
	LCD_SetFont(&Font20);		
	LCD_DisplayStringAt(292, 204, "EX",LEFT_MODE);
	LCD_DisplayStringAt(292, 222, "IT",LEFT_MODE);

	LCD_SetFont(&Font24);
	LCD_FillRect(47, 13, 225, 28);
	LCD_DisplayStringAt(0, 15, "Level Choices", CENTER_MODE);	

	LCD_SetTextColor(GREEN);
	LCD_FillRect(47, 68, 44, 44);
	LCD_SetTextColor(BLACK);
	LCD_FillRect(49, 70, 40, 40);  
	LCD_SetTextColor(GREEN);
	LCD_DisplayStringAt(61, 80, "1", LEFT_MODE);  

	LCD_FillRect(138, 68, 44, 44);
	LCD_SetTextColor(BLACK);
	LCD_FillRect(140, 70, 40, 40);  
	LCD_SetTextColor(GREEN);
	LCD_DisplayStringAt(152, 80, "2", LEFT_MODE); 
	
	LCD_FillRect(229, 68, 44, 44);
	LCD_SetTextColor(BLACK);
	LCD_FillRect(231, 70, 40, 40);  
	LCD_SetTextColor(GREEN);
	LCD_DisplayStringAt(240, 80, "3", LEFT_MODE); 
	
	LCD_FillRect(47, 137, 44, 44);
	LCD_SetTextColor(BLACK);
	LCD_FillRect(49, 139, 40, 40);  
	LCD_SetTextColor(GREEN);
	LCD_DisplayStringAt(61, 149, "4", LEFT_MODE); 

	LCD_FillRect(138, 137, 44, 44);
	LCD_SetTextColor(BLACK);
	LCD_FillRect(140, 139, 40, 40);  
	LCD_SetTextColor(GREEN);
	LCD_DisplayStringAt(152, 149, "5", LEFT_MODE); 
	
	LCD_FillRect(229, 137, 44, 44);
	LCD_SetTextColor(BLACK);
	LCD_FillRect(231, 139, 40, 40);  
	LCD_SetTextColor(GREEN);
	LCD_DisplayStringAt(243, 149, "6", LEFT_MODE); 
}

int level_selector(){
	TSC_TouchDet(1);  
	TS_GetState(&Touch);
	touch_x = -1;
	touch_y = -1;
	if (Touch.TouchDetected){
		touch_x = Touch.x;
		touch_y = Touch.y;
		WaitForTouchRelease(6);
	}
	if(touch_x >= 290 && touch_x <= 320 && touch_y >= 202 && touch_y <= 240)
			return 0;
	else if(touch_x >= 47 && touch_x <= 91 && touch_y >= 68 && touch_y <= 112)
			return 2;  // level 1
	else if(touch_x >= 138 && touch_x <= 182 && touch_y >= 68 && touch_y <= 112)
			return 3;  // level 2  
	else if(touch_x >= 229 && touch_x <= 273 && touch_y >= 68 && touch_y <= 112)
			return 4;  // level 3    
	else if(touch_x >= 47 && touch_x <= 91 && touch_y >= 137 && touch_y <= 181)
			return 5;  // level 4    
	else if(touch_x >= 138 && touch_x <= 182 && touch_y >= 137 && touch_y <= 181)
			return 6;  // level 5    
	else if(touch_x >= 229 && touch_x <= 273 && touch_y >= 137 && touch_y <= 181)
			return 7;  // level 6   
	return 17;
}


