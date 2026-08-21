#include <iostream>
#include "Render.h"
#include "Console.h"
#include "Data.h"

char gameScreen[SCREEN_ROW][SCREEN_COL];

void Render()
{
	ClearGameMap();

	// Enemy Draw
	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
	{
		if (enemys[i].visible)
			DrawSprite(enemys[i].row, enemys[i].col, '0');
	}

	// Player Draw
	DrawSprite(player.row, player.col, '#');

	// Player Bullet Draw
	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (playerBullets[i].visible)
			DrawSprite(playerBullets[i].row, playerBullets[i].col, 'B');
	}


	// Screen Buffer Draw
	for (int i = 0; i < SCREEN_ROW; i++)
	{
		MoveConsoleCursor(0, i);
		std::cout << gameScreen[i];
	}

	std::cout << "player row : " << player.row;
	std::cout << " player col : " << player.col;
}

void ClearGameMap()
{
	memset(gameScreen, ' ', sizeof(gameScreen));

	for (int i = 0; i < SCREEN_ROW; i++)
	{
		gameScreen[i][SCREEN_COL - 1] = '\0';
	}
}

void DrawSprite(int row, int col, char sprite)
{
	if (row < 0 || row >= SCREEN_ROW || col < 0 || col >= SCREEN_COL)
		return;

	gameScreen[row][col] = sprite;
}