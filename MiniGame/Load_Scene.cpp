#include "Scene.h"
#include "Data.h"
#include "Console.h"
#include "Render.h"
#include <windows.h>
#include <cstdio>
#pragma comment(lib, "winmm.lib")

// 파일이 열리지 않았을때
#define FILE_OPEN_ERROR		0x00000001

// stage_info.txt 파일 형식 오류
#define STAGE_INFO_FILE_ERROR 0x00000002

// stage.txt 파일 형식 오류
#define STAGE_FILE_ERROR	0x00000004

// enemy_info.txt 파일 형식 오류
#define ENEMY_INFO_FILE_ERROR	0x00000008

// enemy.txt 파일 형식 오류
#define ENEMY_FILE_ERROR	0x00000010

// move_pattern_info.txt 파일 형식 오류
#define MOVE_PATTERN_INFO_FILE_ERROR	0x00000020

int fileError;

extern Scene scene;

FILE* fp;
const char* stageInfoFileName = "stage/stage_info.txt";
const char* enemyInfoFileName = "enemy/enemy_info.txt";

bool firstLoad = true;

struct FileLoadEnemyData
{
	char sprite;

	MoveType moveType;
	ShotType shotType;
	int moveRange;
	int hp;
	int attack;
	int moveSpeed;
	int bulletSpeed;
	float shotCooldown;
	char movePattern;
};

int stageCount;
Stage* stages;

struct MovePattern
{
	char patternId;
	char* pattern;
	int patternSize;
};

int movePatternsCount;
MovePattern* movePatterns;

int enemyDataCount;
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

	if (fileError != 0)
	{
		return;
	}

	// initialize
	memset(enemyBullets, 0, sizeof(enemyBullets));
	memset(playerBullets, 0, sizeof(playerBullets));
	memset(enemys, 0, sizeof(enemys));

	for (int i = 0; i < SCREEN_ROW; i++)
	{
		for (int j = 0; j < SCREEN_COL - 1; j++)
		{
			for (int q = 0; q < enemyDataCount; q++)
			{
				if (stages[currentStage].screen[i][j] == enemyDatas[q].sprite)
				{
					for (int k = 0; k < MAX_ENEMY_COUNT; k++)
					{
						if (!enemys[k].visible)
						{
							enemys[k].sprite = enemyDatas[q].sprite;
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

							for (int w = 0; w < movePatternsCount; w++)
							{
								if (movePatterns[w].patternId == enemyDatas[q].movePattern)
								{
									enemys[k].movePattern = movePatterns[w].pattern;
									enemys[k].movePatternSize = movePatterns[w].patternSize;
									enemys[k].movePatternIndex = 0;
								}
							}
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
	player.moveSpeed = 25;
	player.shotCooldown = 0.1f;
	player.bulletSpeed = 25;

	scene = GAME_SCENE;
}

static void LoadSceneRender()
{
	ClearGameScreen();

	if (fileError == FILE_OPEN_ERROR)
	{
		DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "[Loading Error] File Open Error");
	}
	else if (fileError == STAGE_INFO_FILE_ERROR)
	{
		DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "[Loading Error] Stage Info File Error");
	}
	else if (fileError == STAGE_FILE_ERROR)
	{
		DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "[Loading Error] Stage File Error");
	}
	else if (fileError == ENEMY_INFO_FILE_ERROR)
	{
		DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "[Loading Error] Enemy Info File Error");
	}
	else if (fileError == ENEMY_FILE_ERROR)
	{
		DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "[Loading Error] Enemy File Error");
	}
	else if (fileError == MOVE_PATTERN_INFO_FILE_ERROR)
	{
		DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "[Loading Error] Move Pattern Info File Error");
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
		fileError = FILE_OPEN_ERROR;
		return;
	}

	char buffer[100];

	stageCount = atoi(fgets(buffer, sizeof(buffer), fp));

	if (stageCount <= 0)
	{
		scene = EXIT_SCENE;
		fileError = STAGE_INFO_FILE_ERROR;
		return;
	}

	// free 해야함
	char** stageFileNames = new char* [stageCount];

	for (int i = 0; i < stageCount; i++)
	{
		// 파일 읽다가 특수한 경우 에러처리 필요해보임.
		// free 해야함
		stageFileNames[i] = new char[sizeof(buffer)];
		fgets(buffer, sizeof(buffer), fp);

		// 스테이지 개수가 주어진 파일보다 많을때
		if (feof(fp) && i != stageCount - 1)
		{
			scene = EXIT_SCENE;
			fileError = STAGE_INFO_FILE_ERROR;
			return;
		}
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
			fileError = FILE_OPEN_ERROR;
			return;
		}
		fseek(fp, 0, SEEK_END);
		int size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		size = fread_s(stages[i].screen, sizeof(stages[i].screen), 1, sizeof(stages[i].screen), fp);

		// 스테이지 파일이 규격에 맞지 않음. 줄바꿈 포함 24 * 81 이어야함.
		if (size != SCREEN_ROW * SCREEN_COL)
		{
			scene = EXIT_SCENE;
			fileError = STAGE_FILE_ERROR;
			return;
		}

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
		fileError = FILE_OPEN_ERROR;
		return;
	}

	while (!feof(fp))
	{
		enemyDataCount++;
		fgets(buffer, sizeof(buffer), fp);
	}

	enemyDatas = new FileLoadEnemyData[enemyDataCount];

	fseek(fp, 0, SEEK_SET);

	int index = 0;
	while (!feof(fp))
	{
		if (fgets(buffer, sizeof(buffer), fp) == NULL)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_INFO_FILE_ERROR;
			return;
		}
		char fileName[100];
		int ret = sscanf_s(buffer, "%c %99s",
			&enemyDatas[index].sprite, (unsigned)sizeof(enemyDatas[index].sprite),
			fileName, (unsigned)sizeof(fileName));

		if (ret != 2)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_INFO_FILE_ERROR;
			return;
		}

		FILE* fp2;
		fopen_s(&fp2, fileName, "r");

		if (fp2 == NULL)
		{
			scene = EXIT_SCENE;
			fileError = FILE_OPEN_ERROR;
			return;
		}

		int value;

		fgets(buffer, sizeof(buffer), fp2);
		if (sscanf_s(buffer, "moveType=%d", &value) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}
		enemyDatas[index].moveType = static_cast<MoveType>(value);

		fgets(buffer, sizeof(buffer), fp2);
		if (sscanf_s(buffer, "moveRange=%d", &enemyDatas[index].moveRange) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}

		fgets(buffer, sizeof(buffer), fp2);
		if (sscanf_s(buffer, "shotType=%d", &value) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}
		enemyDatas[index].shotType = static_cast<ShotType>(value);

		fgets(buffer, sizeof(buffer), fp2);
		if (sscanf_s(buffer, "hp=%d", &enemyDatas[index].hp) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}

		fgets(buffer, sizeof(buffer), fp2);
		if (sscanf_s(buffer, "attack=%d", &enemyDatas[index].attack) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}

		fgets(buffer, sizeof(buffer), fp2);
		if (sscanf_s(buffer, "moveSpeed=%d", &enemyDatas[index].moveSpeed) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}

		fgets(buffer, sizeof(buffer), fp2);
		if (sscanf_s(buffer, "bulletSpeed=%d", &enemyDatas[index].bulletSpeed) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}

		fgets(buffer, sizeof(buffer), fp2);
		if (sscanf_s(buffer, "shotCooldown=%f", &enemyDatas[index].shotCooldown) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}

		fgets(buffer, sizeof(buffer), fp2);
		if (sscanf_s(buffer, "movePattern=%c", &enemyDatas[index].movePattern) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}
		fclose(fp2);

		index++;
	}
	fclose(fp);

	fopen_s(&fp, "enemy/move_pattern_info.txt", "r");

	if (fp == NULL)
	{
		scene = EXIT_SCENE;
		fileError = FILE_OPEN_ERROR;
		return;
	}

	// move_pattern_info 줄 수 구하기.
	while (!feof(fp))
	{
		fgets(buffer, sizeof(buffer), fp);
		movePatternsCount++;
	}
	fseek(fp, 0, SEEK_SET);

	movePatterns = new MovePattern[movePatternsCount];

	// move_pattern 읽어오면서 movePatterns에 넣어주기.
	for (int i = 0; i < movePatternsCount; i++)
	{
		if (fgets(buffer, sizeof(buffer), fp) == NULL)
		{
			scene = EXIT_SCENE;
			fileError = MOVE_PATTERN_INFO_FILE_ERROR;
			return;
		}
		char fileName[100];
		int ret = sscanf_s(buffer, "%c %99s",
			&movePatterns[i].patternId, (unsigned)sizeof(movePatterns[i].patternId),
			fileName, (unsigned)sizeof(fileName));

		if (ret != 2)
		{
			scene = EXIT_SCENE;
			fileError = MOVE_PATTERN_INFO_FILE_ERROR;
			return;
		}

		FILE* fp2;
		fopen_s(&fp2, fileName, "r");

		if (fp2 == NULL)
		{
			scene = EXIT_SCENE;
			fileError = FILE_OPEN_ERROR;
			return;
		}

		fseek(fp2, 0, SEEK_END);
		int patternSize = ftell(fp2);
		fseek(fp2, 0, SEEK_SET);
		movePatterns[i].patternSize = patternSize;

		if (patternSize != 0)
		{
			movePatterns[i].pattern = new char[patternSize + 1];
			fgets(movePatterns[i].pattern, patternSize + 1, fp2);
		}

		fclose(fp2);
	}

}