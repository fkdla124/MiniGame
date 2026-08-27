#pragma once
#include <memory.h>

#define SCREEN_COL		81
#define SCREEN_ROW		24

extern char gameScreen[SCREEN_ROW][SCREEN_COL];

extern void DrawGameScreen();
extern void ClearGameScreen();
extern void DrawSprite(int row, int col, char sprite);


// 문자열 리터럴 or 배열을 꽉 채운 문자열만 취급
void DrawTextGameScreen(int row, int col, const char* str);