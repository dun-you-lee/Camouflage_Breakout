#include "stm32f4xx.h"
#include "stm32f4xx_fsmc.h"
#include "stm324xg_lcd_sklin.h"
#include "game.h"

void stm32f4_Hardware_Init(void);
void Default_Calibration(void);
void Driver_GPIO(void);
int life;
uint8_t * gameover = (uint8_t*)0x080C0000;
uint8_t state = 0 ;
uint8_t pre_state = 1 ;
int page = 0;
int pre_page = 0;
uint8_t store_level;

int main(void) {
	stm32f4_Hardware_Init();   
	LCD_Init();             
	LCD_Clear(BLACK);
	Default_Calibration();
	Driver_GPIO();
	RNG_Init();
	while(1){
		// origin screen
		if(state == 0){
			store_level = -1;
			if(pre_state != state)
					start_screen();
			pre_state = state;
			state = control_screen();
		}
		// intoduction
		else if(state == 1){
			if(pre_state != state || page != pre_page)
					overview(page);
			pre_state = state;
			pre_page = page;
			page = control_page(page);
			if(page == 1000){
				 state = 0;
				 page = 0;
				 pre_page = 0;
			}
		}
		// level 1
		else if( state == 2){
			store_level = 1;
			game_init();
			level_1_draw();
			while (1) {
				life = level_1_flow();
				if(life <= 0){
					pre_state = state;
					state = 15;            
					break;
				}
				if(life == 1000){
					pre_state = state;
					state = 16;
					break;
				}
			}
		}
		// level 2
		else if( state == 3){
			store_level = 2;
			game_init();
			level_2_draw();
			while(1){
				life = level_2_flow();
				if(life <= 0){
					pre_state = state;
					state = 15;             
					break;
				}
				if(life == 1000){
					pre_state = state;
					state = 16;
					break;
				}
			}
		}
		// level 3
		else if( state == 4){
			store_level = 3;
			game_init();
			level_3_draw();
			while(1){
				life = level_3_flow();
				if(life <= 0){
					pre_state = state;
					state = 15;             
					break;
				}
				if(life == 1000){
					pre_state = state;
					state = 16;
					break;
				}
			}
		}
		// level 4
		else if( state == 5){
			store_level = 4;
			game_init();
			level_4_draw();
			while(1){
				life = level_4_flow();
				if(life <= 0){
					pre_state = state;
					state = 15;            
					break;
				}
				if(life == 1000){
					pre_state = state;
					state = 16;
					break;
				}
			}
		}
		// level 5
		else if( state == 6){
			store_level = 5;
			game_init();
			level_5_draw();
			while(1){
				life = level_5_flow();
				if(life <= 0){
					pre_state = state;
					state = 15;             
					break;
				}
				if(life == 1000){
					pre_state = state;
					state = 16;
					break;
				}
			}
		}
		// level 6
		else if( state == 7){
			store_level = 6;
			game_init();
			level_6_draw();
			while(1){
				life = level_6_flow();
				if(life <= 0){
					pre_state = state;
					state = 15;             
					break;
				}
				if(life == 1000){
					pre_state = state;
					state = 16;
					break;
				}
			}
		}
		// hidden level
		else if( state == 8){
			store_level = 6;
			game_init();
			hidden_draw();
			while(1){
				life = hidden_flow();
				if(life <= 0){
					pre_state = state;
					state = 15;             
					break;
				}
				if(life == 1000){
					pre_state = state;
					state = 16;
					break;
				}
			}
		}
		//game over
		else if(state == 15){    					
			if(pre_state != state){
				LCD_DrawRGBImage(0, 0, 320, 240, (uint8_t*)0x080c0000);
				LCD_SetTextColor(YELLOW);
				LCD_SetFont(&Font12);
				LCD_DisplayStringAt(0, 225, "Click the screen to return to the homepage", CENTER_MODE);	
			}
			pre_state = state;
			state = game_over();
		}			
		//judge next level
		else if(state == 16){
			pre_state = state; 					
			state = judge_next_level(store_level + 1);
		}		
		// level selectors
		else if(state == 17){
			if(pre_state != state)
					level_selector_screen();
			pre_state = state;
			state = level_selector();
		}			
	}
}