#include <iostream>
#include "Render.h"
#include "Console.h"
#include "Data.h"

char gameScreen[SCREEN_ROW][SCREEN_COL];

extern int logicFPS;
extern int renderFPS;

void DrawGameScreen()
{
	// 테두리 꾸미기용
	// ===================================================================================================================
	char line[SCREEN_COL + 2];
	memset(line, '=', sizeof(line));
	line[sizeof(line) - 1] = '\0';

	char* ptr;
	switch (scene)
	{
		case TITLE_SCENE:
			memcpy(&line[sizeof(line) / 2 - (sizeof(" TITLE ") - 1) / 2], " TITLE ", sizeof(" TITLE ") - 1);
			break;
		case LOAD_SCENE:
			memcpy(&line[sizeof(line) / 2 - (sizeof(" LOADING ") - 1) / 2], " LOADING ", sizeof(" LOADING ") - 1);
			break;
		case GAME_SCENE:
			ptr = (char*)memcpy(&line[sizeof(line) / 2 - (sizeof(" STAGE ") - 1) / 2], " STAGE ", sizeof(" STAGE ") - 1);
			ptr += sizeof(" STAGE ") - 1;
			*ptr++ = currentStage + '0' + 1;
			*ptr = ' ';
			break;
		case GAME_OVER_SCENE:
			memcpy(&line[sizeof(line) / 2 - (sizeof(" GAME OVER ") - 1) / 2], " GAME OVER ", sizeof(" GAME OVER ") - 1);
			break;
		case GAME_CLEAR_SCENE:
			memcpy(&line[sizeof(line) / 2 - (sizeof(" GAME CLEAR ") - 1) / 2], " GAME CLEAR ", sizeof(" GAME CLEAR ") - 1);
			break;
	}
	MoveConsoleCursor(0, 0);
	std::cout << line;
	// ===================================================================================================================




	// 게임 화면 그리기
	// ===================================================================================================================
	for (int i = 0; i < SCREEN_ROW; i++)
	{
		MoveConsoleCursor(0, i + 1);
		std::cout << "|" << gameScreen[i] << "|";
	}
	// ===================================================================================================================





	// 테두리 꾸미기용
	// ===================================================================================================================
	MoveConsoleCursor(0, SCREEN_ROW + 1);
	memset(line, '=', sizeof(line));
	line[sizeof(line) - 1] = '\0';
	std::cout << line;
	MoveConsoleCursor(0, SCREEN_ROW + 2);
	std::cout << " Move : arrows";

	MoveConsoleCursor(0, SCREEN_ROW + 3);
	std::cout << " Shot : z";

	std::cout << "               HP : ";
	for (int i = 0; i < player.hp; i++)
	{
		std::cout << " / ";
	}
	std::cout << "      ";

	MoveConsoleCursor(SCREEN_COL - sizeof(" Logic FPS :   "), SCREEN_ROW + 2);
	std::cout << " Logic FPS : " << logicFPS << "    ";
	MoveConsoleCursor(SCREEN_COL - sizeof(" Render FPS :   "), SCREEN_ROW + 3);
	std::cout << " Render FPS : " << renderFPS << "    ";
	// ===================================================================================================================
}

void ClearGameScreen()
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

void DrawTextGameScreen(int row, int col, const char* str)
{
	if (row < 0 || row >= SCREEN_ROW || col < 0 || col >= SCREEN_COL - 1)
		return;

	int length = 0;

	while (*str != '\0')
	{
		length++;
		str++;
	}
	str -= length;

	if (length > ((SCREEN_COL - 1) - col))
		length = ((SCREEN_COL - 1) - col);

	memcpy(&gameScreen[row][col], str, length);
}