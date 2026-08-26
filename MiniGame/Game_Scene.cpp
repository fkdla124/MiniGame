#include "Game.h"
#include <cmath>

static void GameSceneInput();
static void GameSceneLogic();
static void GameSceneRender();

// Logic
static void ShotEnemy();
static void MoveEnemy();
static void MovePlayerBullet();
static void CreatePlayerBullet();
static void PlayerBulletCollision();
static void CheckGameClear();
static void MoveEnemyBullet();
static void CreateEnemyBullet(Enemy* enemy, int drow, int dcol);
static void EnemyBulletCollision();

static void UpdateTick();
static void UpdatePlayer();
static void UpdateEnemy();
static void UpdatePlayerBullets();
static void UpdateEnemyBullets();
static void CheckCollision();

unsigned long tick = timeGetTime();
static float deltaTime;

void GameScene()
{
	GameSceneInput();

	GameSceneLogic();

	GameSceneRender();
}

static void GameSceneInput()
{
	if (player.moveTimer >= 50 / player.moveSpeed)
	{
		if (GetAsyncKeyState(VK_LEFT))
		{
			player.col = max(player.col - 1, 0);
		}
		if (GetAsyncKeyState(VK_RIGHT))
		{
			player.col = min(player.col + 1, SCREEN_COL - 2);
		}
		if (GetAsyncKeyState(VK_UP))
		{
			player.row = max(player.row - 1, 0);
		}
		if (GetAsyncKeyState(VK_DOWN))
		{
			player.row = min(player.row + 1, SCREEN_ROW - 1);
		}

		player.moveTimer = 0;
	}

	// 플레이어 총알 발사
	if (GetAsyncKeyState('Z'))
	{
		CreatePlayerBullet();
	}

	// 총알 제거
	if (GetAsyncKeyState('C'))
	{
		memset(enemyBullets, 0, sizeof(enemyBullets));
	}

	// 적 + 총알 제거
	if (GetAsyncKeyState('X') & 0x0001)
	{
		memset(enemyBullets, 0, sizeof(enemyBullets));
		memset(enemys, 0, sizeof(enemys));
	}
}

static void GameSceneLogic()
{
	UpdateTick();

	UpdatePlayer();
	UpdateEnemy();
	UpdatePlayerBullets();
	UpdateEnemyBullets();

	CheckCollision();

	CheckGameClear();
}

static void GameSceneRender()
{
	// 화면 비우기
	ClearGameScreen();

	// 플레이어 총알 그리기
	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (playerBullets[i].visible)
			DrawSprite(playerBullets[i].row, playerBullets[i].col, '|');
	}

	// 적 총알 그리기
	for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
	{
		if (enemyBullets[i].visible)
			DrawSprite(enemyBullets[i].row, enemyBullets[i].col, '*');
	}

	// 적 그리기
	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
	{
		if (enemys[i].visible)
			DrawSprite(enemys[i].row, enemys[i].col, enemys[i].sprite);
	}

	// 플레이어 그리기
	DrawSprite(player.row, player.col, 'P');

	// 화면 출력
	DrawGameScreen();
}

static void UpdatePlayer()
{
	player.moveTimer++;
	player.shotTimer += deltaTime;
}
static void UpdateEnemy()
{
	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
	{
		if (enemys[i].visible)
		{
			enemys[i].moveTimer++;
			enemys[i].shotTimer += deltaTime;
		}
	}

	MoveEnemy();
	ShotEnemy();
}
static void UpdatePlayerBullets()
{
	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (playerBullets[i].visible)
		{
			playerBullets[i].moveTimer++;
		}
	}

	MovePlayerBullet();
}
static void UpdateEnemyBullets()
{
	for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
	{
		if (enemyBullets[i].visible)
		{
			enemyBullets[i].moveTimer++;
		}
	}

	MoveEnemyBullet();
}

static void CheckCollision()
{
	PlayerBulletCollision();
	EnemyBulletCollision();
}

static void CheckGameClear()
{
	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
	{
		if (enemys[i].visible)
			return;
	}


	if (currentStage == (stageCount - 1))
	{
		scene = GAME_CLEAR_SCENE;
	}
	else
	{
		currentStage++;
		scene = LOAD_SCENE;
	}
}

static void MoveEnemy()
{
	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
	{
		if (!enemys[i].visible || enemys[i].movePatternSize == 0 || enemys[i].moveTimer < (50 / enemys[i].moveSpeed))
			continue;

		switch (enemys[i].movePattern[enemys[i].movePatternIndex])
		{
			case 'L':
				enemys[i].col = max(enemys[i].col - 1, 0);
				break;
			case 'R':
				enemys[i].col = min(enemys[i].col + 1, SCREEN_COL - 2);
				break;
			case 'U':
				enemys[i].row = max(enemys[i].row - 1, 0);
				break;
			case 'D':
				enemys[i].row = min(enemys[i].row + 1, SCREEN_ROW - 1);
				break;
		}

		enemys[i].movePatternIndex = (enemys[i].movePatternIndex + 1) % enemys[i].movePatternSize;
		enemys[i].moveTimer = 0;
	}
}

static void ShotEnemy()
{
	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
	{
		if (!enemys[i].visible || enemys[i].shotTimer < enemys[i].shotCooldown)
			continue;

		switch (enemys[i].shotType)
		{
			case ShotType::DOWN:
				CreateEnemyBullet(&enemys[i], 1, 0);
				break;
			case ShotType::FOUR_WAY:
				CreateEnemyBullet(&enemys[i], 0, -1);
				CreateEnemyBullet(&enemys[i], 0, 1);
				CreateEnemyBullet(&enemys[i], -1, 0);
				CreateEnemyBullet(&enemys[i], 1, 0);
				break;
			case ShotType::EIGHT_WAY:
				CreateEnemyBullet(&enemys[i], 0, -1);
				CreateEnemyBullet(&enemys[i], -1, -1);
				CreateEnemyBullet(&enemys[i], -1, 0);
				CreateEnemyBullet(&enemys[i], -1, 1);
				CreateEnemyBullet(&enemys[i], 0, 1);
				CreateEnemyBullet(&enemys[i], 1, 1);
				CreateEnemyBullet(&enemys[i], 1, 0);
				CreateEnemyBullet(&enemys[i], 1, -1);
				break;
		}

		enemys[i].shotTimer = 0;
	}
}

static void MovePlayerBullet()
{
	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (!playerBullets[i].visible)
			continue;

		// 다음 좌표가 화면밖이면 지우기
		if (playerBullets[i].row - 1 < 0)
		{
			playerBullets[i].visible = false;
			continue;
		}

		// 이동 로직
		if (playerBullets[i].moveTimer >= (50 / playerBullets[i].moveSpeed))
		{
			playerBullets[i].moveTimer = 0;
			playerBullets[i].row -= 1;
		}
	}
}

static void CreatePlayerBullet()
{
	if (player.shotTimer < player.shotCooldown)
		return;

	int count = 3; // 한번에 생성할 총알.
	int offset = 0;
	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (playerBullets[i].visible)
			continue;

		int createRow = player.row - 1;
		int createCol = player.col - ((count - 1) / 2) + offset;

		if (createRow < 0 || createRow >= SCREEN_ROW || createCol < 0 || createCol >= SCREEN_COL - 1)
		{
			offset++;
			if (offset == count)
			{
				player.shotTimer = 0.0f;
				break;
			}

			continue;
		}

		playerBullets[i].visible = true;
		playerBullets[i].row = createRow;
		playerBullets[i].col = createCol;
		playerBullets[i].damage = player.attack;
		playerBullets[i].moveSpeed = player.bulletSpeed;
		playerBullets[i].moveTimer = 0;

		offset++;
		if (offset == count)
		{
			player.shotTimer = 0.0f;
			break;
		}
	}
}

static void PlayerBulletCollision()
{
	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (!playerBullets[i].visible)
			continue;

		for (int j = 0; j < MAX_ENEMY_COUNT; j++)
		{
			if (!enemys[j].visible)
				continue;

			if (playerBullets[i].row == enemys[j].row && playerBullets[i].col == enemys[j].col)
			{
				enemys[j].hp -= playerBullets[i].damage;

				playerBullets[i].visible = false;

				if (enemys[j].hp <= 0)
					enemys[j].visible = false;
			}
		}
	}
}

static void MoveEnemyBullet()
{
	for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
	{
		if (!enemyBullets[i].visible)
			continue;

		// 이동 프레임 제한
		if (enemyBullets[i].moveTimer < (50 / enemyBullets[i].moveSpeed))
			continue;

		// 다음 좌표가 화면밖이면 삭제하고 continue
		if (enemyBullets[i].col - 1 < 0 || enemyBullets[i].row - 1 < 0 || enemyBullets[i].row + 1 >= SCREEN_ROW || enemyBullets[i].col + 1 >= SCREEN_COL - 1)
		{
			enemyBullets[i].visible = false;
			continue;
		}

		// 이동 로직
		enemyBullets[i].row += enemyBullets[i].drow;
		enemyBullets[i].col += enemyBullets[i].dcol;
		enemyBullets[i].moveTimer = 0;
	}
}

static void CreateEnemyBullet(Enemy* enemy, int drow, int dcol)
{
	for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
	{
		if (!enemyBullets[i].visible)
		{
			enemyBullets[i].visible = true;
			enemyBullets[i].row = enemy->row + drow;
			enemyBullets[i].col = enemy->col + dcol;
			enemyBullets[i].damage = enemy->attack;
			enemyBullets[i].moveSpeed = enemy->bulletSpeed;
			enemyBullets[i].drow = drow;
			enemyBullets[i].dcol = dcol;
			enemyBullets[i].moveTimer = 0;
			break;
		}
	}
}

static void EnemyBulletCollision()
{
	for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
	{
		if (!enemyBullets[i].visible)
			continue;

		if (enemyBullets[i].row == player.row && enemyBullets[i].col == player.col)
		{
			player.hp -= enemyBullets[i].damage;

			enemyBullets[i].visible = false;

			if (player.hp <= 0)
			{
				player.visible = false;
				scene = GAME_OVER_SCENE;
			}

			return;
		}
	}
}

static void UpdateTick()
{
	unsigned long diff = timeGetTime() - tick;
	deltaTime = static_cast<float>(diff) / 1000.0f;
	tick = timeGetTime();
}