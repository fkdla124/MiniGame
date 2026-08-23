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
const char* enemyInfoFileName = "enemy/enemy_info.txt";
bool openFile = true;

bool firstLoad = true;

struct FileLoadEnemyData
{
	char enemyId;

	MoveType moveType;
	ShotType shotType;
	int moveRange;
	int hp;
	int attack;
	int moveSpeed;
	int bulletSpeed;
	float shotCooldown;
};

int stageCount;
Stage* stages;
int enemyTypeCount;
FileLoadEnemyData* enemyDatas;

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

	for (int i = 0; i < SCREEN_ROW; i++)
	{
		for (int j = 0; j < SCREEN_COL - 1; j++)
		{
			for (int q = 0; q < enemyTypeCount; q++)
			{
				if (stages[currentStage].screen[i][j] == enemyDatas[q].enemyId)
				{
					for (int k = 0; k < MAX_ENEMY_COUNT; k++)
					{
						if (!enemys[k].visible)
						{
							enemys[k].moveType = enemyDatas[q].moveType;
							enemys[k].shotType = enemyDatas[q].shotType;
							enemys[k].hp = enemyDatas[q].hp;
							enemys[k].attack = enemyDatas[q].attack;
							enemys[k].moveSpeed = enemyDatas[q].moveSpeed;
							enemys[k].bulletSpeed = enemyDatas[q].bulletSpeed;
							enemys[k].shotCooldown = enemyDatas[q].shotCooldown;
							enemys[k].moveRange = enemyDatas[q].moveRange;
							enemys[k].spawnRow = i;
							enemys[k].spawnCol = j;
							enemys[k].row = i;
							enemys[k].col = j;
							enemys[k].visible = true;
							break;
						}
					}
				}
			}
		}
	}

	// enemy create
	//TestEnemySet();

	// player create
	if (currentStage == 0)
	{
		memset(&player, 0, sizeof(player));
		player.hp = 5;
	}
	player.row = SCREEN_ROW - 3;
	player.col = SCREEN_COL / 2;
	player.visible = true;
	player.attack = 1;
	player.moveSpeed = 20;
	player.shotCooldown = 0.1f;
	player.bulletSpeed = 20;

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

	fopen_s(&fp, enemyInfoFileName, "r");

	if (fp == NULL)
	{
		scene = EXIT_SCENE;
		openFile = false;
		return;
	}

	while (!feof(fp))
	{
		enemyTypeCount++;
		fgets(buffer, sizeof(buffer), fp);
	}

	enemyDatas = new FileLoadEnemyData[enemyTypeCount];

	fseek(fp, 0, SEEK_SET);

	int index = 0;
	while (!feof(fp))
	{
		fgets(buffer, sizeof(buffer), fp);
		char fileName[30];
		int ret = sscanf_s(buffer, " %c %29s",
			&enemyDatas[index].enemyId, (unsigned)sizeof(enemyDatas[index].enemyId),
			fileName, (unsigned)sizeof(fileName));

		if (ret != 2) {
			// 파싱 실패 처리
			return;
		}

		FILE* fp2;
		fopen_s(&fp2, fileName, "r");

		if (fp2 == NULL)
		{
			scene = EXIT_SCENE;
			openFile = false;
			return;
		}

		int value;
		fgets(buffer, sizeof(buffer), fp2);
		sscanf_s(buffer, "moveType=%d", &value);
		enemyDatas[index].moveType = static_cast<MoveType>(value);
		fgets(buffer, sizeof(buffer), fp2);
		sscanf_s(buffer, "moveRange=%d", &enemyDatas[index].moveRange);
		fgets(buffer, sizeof(buffer), fp2);
		sscanf_s(buffer, "shotType=%d", &value);
		enemyDatas[index].shotType = static_cast<ShotType>(value);
		fgets(buffer, sizeof(buffer), fp2);
		sscanf_s(buffer, "hp=%d", &enemyDatas[index].hp);
		fgets(buffer, sizeof(buffer), fp2);
		sscanf_s(buffer, "attack=%d", &enemyDatas[index].attack);
		fgets(buffer, sizeof(buffer), fp2);
		sscanf_s(buffer, "moveSpeed=%d", &enemyDatas[index].moveSpeed);
		fgets(buffer, sizeof(buffer), fp2);
		sscanf_s(buffer, "bulletSpeed=%d", &enemyDatas[index].bulletSpeed);
		fgets(buffer, sizeof(buffer), fp2);
		sscanf_s(buffer, "shotCooldown=%f", &enemyDatas[index].shotCooldown);
		fclose(fp2);

		index++;
	}
	fclose(fp);
}