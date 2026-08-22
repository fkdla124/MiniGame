#pragma once
#include <memory.h>


#define SCREEN_COL		81
#define SCREEN_ROW		24

extern char gameScreen[SCREEN_ROW][SCREEN_COL];

extern void DrawGameScreen();
extern void ClearGameScreen();
extern void DrawSprite(int row, int col, char sprite);

template <size_t N>
void DrawTextGameScreen(int row, int col, const char(&text)[N])
{
	if (row < 0 || row >= SCREEN_ROW || col < 0 || col >= SCREEN_COL - 1)
		return;

	int length = N - 1;

	if ((N - 1) > ((SCREEN_COL - 1) - col))
		length = ((SCREEN_COL - 1) - col);

	memcpy(&gameScreen[row][col], text, length);
}