#ifndef GAME_H
#define GAME_H
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm324xg_lcd_sklin.h"  // already defines Point {X,Y} and COLOR_* macros
#include "touch_module.h"
#include "rng.h"
#define SCREEN_W    320
#define SCREEN_H    240
#define BTN_SIZE     50
#define MARGIN       10
#define STEP         5
#ifdef __cplusplus
extern "C" {
#endif

Point GetPlayerPos(void);
void start_screen(void);
uint8_t control_screen(void);
uint8_t game_over(void);
uint8_t judge_next_level(int);
void overview(int);
int control_page(int);
void game_init(void);
void level_1_draw(void);
int level_1_flow(void);
int level_2_flow(void);
void level_2_draw(void);
void level_3_draw(void);
int level_3_flow(void);
void level_4_draw(void);
int level_4_flow(void);
void level_5_draw(void);
int level_5_flow(void);
void level_6_draw(void);
int level_6_flow(void);
void hidden_draw(void);
int hidden_flow(void);
int level_selector(void);
void level_selector_screen(void);

#ifdef __cplusplus
}
#endif

#endif // GAME_H
