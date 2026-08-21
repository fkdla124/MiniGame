#pragma once

#define SCREEN_COL		81
#define SCREEN_ROW		24

extern char gameScreen[SCREEN_ROW][SCREEN_COL];

extern void Render();
extern void ClearGameMap();
extern void DrawSprite(int row, int col, char sprite);