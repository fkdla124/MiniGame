#include "Scene.h"
#include "Data.h"
#include "Console.h"
#include "Render.h"
#include <windows.h>
#include <cstdio>
#pragma comment(lib, "winmm.lib")

extern Scene scene;

FILE* fp;
const char* stageInfoFileName = "stage/stage_info.txt";
bool openFile = true;

bool firstLoad = true;

int stageCount;
Stage* stages;

static void LoadSceneInput();
static void LoadSceneLogic();
static void LoadSceneRender();

static void LoadGameData();

void LoadScene()
{
	LoadSceneInput();

	LoadSceneLogic();

	LoadSceneRender();
}

static void LoadSceneInput()
{

}

static void LoadSceneLogic()
{
	if (firstLoad)
	{
		LoadGameData();
		firstLoad = false;
	}

	// initialize
	memset(enemyBullets, 0, sizeof(enemyBullets));
	memset(playerBullets, 0, sizeof(playerBullets));
	memset(enemys, 0, sizeof(enemys));
	memset(&player, 0, sizeof(player));

	for (int i = 0; i < SCREEN_ROW; i++)
	{
		for (int j = 0; j < SCREEN_COL - 1; j++)
		{
			if (stages[currentStage].screen[i][j] == '1')
			{
				for (int k = 0; k < MAX_ENEMY_COUNT; k++)
				{
					if (!enemys[k].visible)
					{
						enemys[k].row = i;
						enemys[k].col = j;
						enemys[k].hp = 100;
						enemys[k].visible = true;
						break;
					}
				}
			}
		}
	}

	// enemy create
	//TestEnemySet();

	// player create
	player.row = SCREEN_ROW - 3;
	player.col = SCREEN_COL / 2;
	player.visible = true;
	player.hp = 100;

	scene = GAME_SCENE;
}

static void LoadSceneRender()
{
	ClearGameScreen();

	if (!openFile)
	{
		// hard coding
		DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "[Load Error] no exist file");
	}
	else
	{
		DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "Loading...");
	}

	DrawGameScreen();
}

static void LoadGameData()
{
	fopen_s(&fp, stageInfoFileName, "r");

	if (fp == NULL)
	{
		scene = EXIT_SCENE;
		openFile = false;
		return;
	}

	char buffer[30];

	stageCount = atoi(fgets(buffer, sizeof(buffer), fp));

	// free 해야함
	char** stageFileNames = new char* [stageCount];

	for (int i = 0; i < stageCount; i++)
	{
		// 파일 읽다가 특수한 경우 에러처리 필요해보임.
		// free 해야함
		stageFileNames[i] = new char[sizeof(buffer)];
		fgets(buffer, sizeof(buffer), fp);
		buffer[strcspn(buffer, "\n")] = '\0';
		memcpy(stageFileNames[i], buffer, sizeof(buffer));
	}

	fclose(fp);

	stages = new Stage[stageCount];

	for (int i = 0; i < stageCount; i++)
	{
		fopen_s(&fp, stageFileNames[i], "r");
		if (fp == NULL)
		{
			scene = EXIT_SCENE;
			openFile = false;
			return;
		}
		fseek(fp, 0, SEEK_END);
		int size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		fread_s(stages[i].screen, sizeof(stages[i].screen), 1, sizeof(stages[i].screen), fp);
		fclose(fp);
	}

	for (int i = 0; i < stageCount; i++)
	{
		delete[] stageFileNames[i];
	}
	delete[] stageFileNames;
}