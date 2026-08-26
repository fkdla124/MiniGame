#include "Game.h"

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

bool firstLoad = true;

struct FileLoadEnemyData
{
	char sprite;

	ShotType shotType;
	int hp;
	int attack;
	int moveSpeed;
	int bulletSpeed;
	float shotCooldown;
	char movePattern;
};

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

int stageCount;
Stage* stages;

static void LoadSceneInput();
static void LoadSceneLogic();
static void LoadSceneRender();

static void LoadGameData();

static void InitEnemy();
static void InitPlayer();

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
	// 첫 로딩에만 파일 읽기
	if (firstLoad)
	{
		LoadGameData();
		firstLoad = false;
	}

	// 파일 에러면 오류출력후 프로그램 종료
	if (fileError != 0)
	{
		return;
	}

	// 초기화
	memset(enemyBullets, 0, sizeof(enemyBullets));
	memset(playerBullets, 0, sizeof(playerBullets));
	memset(enemys, 0, sizeof(enemys));

	// 파일에서 읽은걸로 적 생성
	InitEnemy();

	// 플레이어 생성
	InitPlayer();
}

static void LoadSceneRender()
{
	ClearGameScreen();

	switch (fileError)
	{
		case FILE_OPEN_ERROR:
			DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "[Loading Error] File Open Error");
			break;

		case STAGE_INFO_FILE_ERROR:
			DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "[Loading Error] Stage Info File Error");
			break;

		case STAGE_FILE_ERROR:
			DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "[Loading Error] Stage File Error");
			break;

		case ENEMY_INFO_FILE_ERROR:
			DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "[Loading Error] Enemy Info File Error");
			break;

		case ENEMY_FILE_ERROR:
			DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "[Loading Error] Enemy File Error");
			break;

		case MOVE_PATTERN_INFO_FILE_ERROR:
			DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "[Loading Error] Move Pattern Info File Error");
			break;

		default:
			DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "Loading...");
			break;

	}

	DrawGameScreen();
}

static void LoadGameData()
{
	FILE* fp;

	// stage_info.txt 파일 작업
	// ==============================================================================================================
	fopen_s(&fp, "stage/stage_info.txt", "r");

	if (fp == NULL)
	{
		scene = EXIT_SCENE;
		fileError = FILE_OPEN_ERROR;
		return;
	}

	fseek(fp, 0, SEEK_END);
	int stageInfoFilesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* stageInfoFileBuffer = new char[stageInfoFilesize];
	fread(stageInfoFileBuffer, 1, stageInfoFilesize, fp);

	fclose(fp);
	// ==============================================================================================================





	// stage.txt 파일 작업
	// ==============================================================================================================
	char* stageInfoFilePtr = stageInfoFileBuffer;
	int stageInfoFileOffset = 0;

	if (sscanf_s(stageInfoFilePtr, "%d%n", &stageCount, &stageInfoFileOffset) != 1)
	{
		scene = EXIT_SCENE;
		fileError = STAGE_INFO_FILE_ERROR;
		return;
	}
	stageInfoFilePtr += stageInfoFileOffset;

	stages = new Stage[stageCount];
	for (int i = 0; i < stageCount; i++)
	{
		char stageFileName[100]; // 파일명 100글자 이내라고 가정

		if (sscanf_s(stageInfoFilePtr, "%99s%n", stageFileName, (unsigned)sizeof(stageFileName), &stageInfoFileOffset) != 1)
		{
			scene = EXIT_SCENE;
			fileError = STAGE_INFO_FILE_ERROR;
			return;
		}
		stageFileName[sizeof(stageFileName) - 1] = '\0';
		stageInfoFilePtr += stageInfoFileOffset;

		fopen_s(&fp, stageFileName, "r");

		if (fp == NULL)
		{
			scene = EXIT_SCENE;
			fileError = FILE_OPEN_ERROR;
			return;
		}

		if (fread(stages[i].screen, 1, sizeof(stages[i].screen), fp) < sizeof(stages[i].screen) - 1)
		{
			scene = EXIT_SCENE;
			fileError = STAGE_FILE_ERROR;
			return;
		}

		fclose(fp);
	}

	delete[] stageInfoFileBuffer;
	// ==============================================================================================================





	// enemy_info.txt 작업
	// ==============================================================================================================
	fopen_s(&fp, "enemy/enemy_info.txt", "r");

	if (fp == NULL)
	{
		scene = EXIT_SCENE;
		fileError = FILE_OPEN_ERROR;
		return;
	}

	// enemy.txt 개수 세기
	while (!feof(fp))
	{
		char buffer[100];
		fgets(buffer, sizeof(buffer), fp);
		enemyDataCount++;
	}
	int enemyInfoFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	enemyDatas = new FileLoadEnemyData[enemyDataCount];

	// enemy_info.txt 통으로 읽기
	char* enemyInfoFileBuffer = new char[enemyInfoFileSize];
	fread(enemyInfoFileBuffer, 1, enemyInfoFileSize, fp);

	fclose(fp);
	// ==============================================================================================================





	// enemy.txt 작업
	// ==============================================================================================================
	char* enemyInfoFilePtr = enemyInfoFileBuffer;
	int enemyInfoFileOffset = 0;

	for (int i = 0; i < enemyDataCount; i++)
	{
		char enemyFileName[100];

		if (sscanf_s(enemyInfoFilePtr, "%c %99s\n%n", &enemyDatas[i].sprite, 1U, enemyFileName, (unsigned)sizeof(enemyFileName), &enemyInfoFileOffset) != 2)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_INFO_FILE_ERROR;
			return;
		}
		enemyInfoFilePtr += enemyInfoFileOffset;
		enemyFileName[sizeof(enemyFileName) - 1] = '\0';

		fopen_s(&fp, enemyFileName, "r");

		if (fp == NULL)
		{
			scene = EXIT_SCENE;
			fileError = FILE_OPEN_ERROR;
			return;
		}

		fseek(fp, 0, SEEK_END);
		int enemyFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);


		char* enemyFileBuffer = new char[enemyFileSize];
		fread(enemyFileBuffer, 1, enemyFileSize, fp);
		fclose(fp);

		char* enemyFilePtr = enemyFileBuffer;
		int enemyFileOffset = 0;
		if (sscanf_s(enemyFilePtr, "shotType=%d\n%n", &enemyDatas[i].shotType, &enemyFileOffset) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}
		enemyFilePtr += enemyFileOffset;

		if (sscanf_s(enemyFilePtr, "hp=%d\n%n", &enemyDatas[i].hp, &enemyFileOffset) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}
		enemyFilePtr += enemyFileOffset;

		if (sscanf_s(enemyFilePtr, "attack=%d\n%n", &enemyDatas[i].attack, &enemyFileOffset) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}
		enemyFilePtr += enemyFileOffset;

		if (sscanf_s(enemyFilePtr, "moveSpeed=%d\n%n", &enemyDatas[i].moveSpeed, &enemyFileOffset) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}
		enemyFilePtr += enemyFileOffset;

		if (sscanf_s(enemyFilePtr, "bulletSpeed=%d\n%n", &enemyDatas[i].bulletSpeed, &enemyFileOffset) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}
		enemyFilePtr += enemyFileOffset;

		if (sscanf_s(enemyFilePtr, "shotCooldown=%f\n%n", &enemyDatas[i].shotCooldown, &enemyFileOffset) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}
		enemyFilePtr += enemyFileOffset;

		if (sscanf_s(enemyFilePtr, "movePattern=%c%n", &enemyDatas[i].movePattern, 1U, &enemyFileOffset) != 1)
		{
			scene = EXIT_SCENE;
			fileError = ENEMY_FILE_ERROR;
			return;
		}
		enemyFilePtr += enemyFileOffset;

		delete[] enemyFileBuffer;
	}
	// ==============================================================================================================





	// move_pattern_info.txt 작업
	// ==============================================================================================================
	fopen_s(&fp, "enemy/move_pattern_info.txt", "r");

	if (fp == NULL)
	{
		scene = EXIT_SCENE;
		fileError = FILE_OPEN_ERROR;
		return;
	}

	while (!feof(fp))
	{
		char buffer[100];
		fgets(buffer, sizeof(buffer), fp);
		movePatternsCount++;
	}
	int movePatternInfoFileSize = ftell(fp);
	char* movePatternInfoFileBuffer = new char[movePatternInfoFileSize];

	fseek(fp, 0, SEEK_SET);
	fread(movePatternInfoFileBuffer, 1, movePatternInfoFileSize, fp);

	movePatterns = new MovePattern[movePatternsCount];

	fclose(fp);
	// ==============================================================================================================





	// move_pattern.txt 작업
	// ==============================================================================================================
	char* movePatternInfoFilePtr = movePatternInfoFileBuffer;
	int movePatternInfoFileOffset = 0;
	for (int i = 0; i < movePatternsCount; i++)
	{
		char movePatternFileName[100];

		if (sscanf_s(movePatternInfoFilePtr, "%c %99s\n%n", &movePatterns[i].patternId, 1U, movePatternFileName, (unsigned)sizeof(movePatternFileName), &movePatternInfoFileOffset) != 2)
		{
			scene = EXIT_SCENE;
			fileError = MOVE_PATTERN_INFO_FILE_ERROR;
			return;
		}
		movePatternInfoFilePtr += movePatternInfoFileOffset;

		movePatternFileName[sizeof(movePatternFileName) - 1] = '\0';

		fopen_s(&fp, movePatternFileName, "r");

		if (fp == NULL)
		{
			scene = EXIT_SCENE;
			fileError = FILE_OPEN_ERROR;
			return;
		}

		fseek(fp, 0, SEEK_END);
		int patternSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		movePatterns[i].patternSize = patternSize;

		if (patternSize != 0)
		{
			movePatterns[i].pattern = new char[patternSize + 1];
			fgets(movePatterns[i].pattern, patternSize + 1, fp);
		}

		fclose(fp);
	}
	// ==============================================================================================================
}

static void InitEnemy()
{
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
							enemys[k].shotType = enemyDatas[q].shotType;
							enemys[k].hp = enemyDatas[q].hp;
							enemys[k].attack = enemyDatas[q].attack;
							enemys[k].moveSpeed = enemyDatas[q].moveSpeed;
							enemys[k].bulletSpeed = enemyDatas[q].bulletSpeed;
							enemys[k].shotCooldown = enemyDatas[q].shotCooldown;
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
}

static void InitPlayer()
{
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