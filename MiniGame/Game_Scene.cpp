#include "Game.h"

static void GameSceneInput();
static void GameSceneLogic();
static void GameSceneRender();

// Logic
static void CreateEnemy(int row, int col);
void TestEnemySet();
static void EnemyAI();
static void MovePlayerBullet();
static void CreatePlayerBullet();
static void PlayerBulletCollision();
static void GameClearCheck();
static void MoveEnemyBullet();
static void CreateEnemyBullet();
static void EnemyBulletCollision();

static void CreateEnemy(int row, int col);

void GameScene()
{
	GameSceneInput();

	GameSceneLogic();

	GameSceneRender();
}

static void GameSceneInput()
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
	if (GetAsyncKeyState('Z'))
	{
		CreatePlayerBullet();
	}
}

static void GameSceneLogic()
{
	MovePlayerBullet();
	PlayerBulletCollision();

	EnemyAI();
	MoveEnemyBullet();
	EnemyBulletCollision();


	GameClearCheck();
}

static void GameClearCheck()
{
	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
	{
		if (enemys[i].visible)
		{
			return;
		}
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

static void GameSceneRender()
{
	ClearGameScreen();

	// Player Bullet Draw
	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (playerBullets[i].visible)
			DrawSprite(playerBullets[i].row, playerBullets[i].col, '|');
	}

	// Enemy Bullet Draw
	for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
	{
		if (enemyBullets[i].visible)
			DrawSprite(enemyBullets[i].row, enemyBullets[i].col, '*');
	}

	// Enemy Draw
	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
	{
		if (enemys[i].visible)
			DrawSprite(enemys[i].row, enemys[i].col, 'E');
	}

	// Player Draw
	DrawSprite(player.row, player.col, 'P');

	// Game Screen Draw
	DrawGameScreen();

	//std::cout << "player row : " << player.row;
	//std::cout << " player col : " << player.col;
}

static void CreateEnemy(int row, int col)
{
	if (row < 0 || row >= SCREEN_ROW || col < 0 || col >= SCREEN_COL)
		return;

	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
	{
		if (!enemys[i].visible)
		{
			enemys[i].visible = true;
			enemys[i].row = row;
			enemys[i].col = col;
			enemys[i].hp = 100;

			return;
		}
	}
}

// test!!
void TestEnemySet()
{
	for (int i = 0; i < 10; i++)
	{
		CreateEnemy(5, 20 + i * 5);
	}
	for (int i = 0; i < 10; i++)
	{
		CreateEnemy(10, 20 + i * 5);
	}
}

static void EnemyAI()
{
	const int moveWidth = 10;

	// 0 left
	// 1 right
	static int moveDirection = 0;
	static int moveCount = 0;
	static int moveFrameCount = 0;

	moveFrameCount = (moveFrameCount + 1) % 3;

	if (moveFrameCount != 2)
	{
		moveFrameCount = 0;

		// move logic
		for (int i = 0; i < MAX_ENEMY_COUNT; i++)
		{
			if (enemys[i].visible)
			{
				if (moveDirection)
					enemys[i].col = min(enemys[i].col + 1, SCREEN_COL - 2);
				else
					enemys[i].col = max(enemys[i].col - 1, 0);
			}
		}

		moveCount++;

		if (moveCount >= moveWidth)
		{
			moveCount = 0;
			moveDirection = moveDirection ? 0 : 1;
		}
	}

	static int shotFrameCount = 0;

	shotFrameCount = (shotFrameCount + 1) % 15;

	if (shotFrameCount == 14)
	{
		shotFrameCount = 0;

		CreateEnemyBullet();
	}
}

static void MovePlayerBullet()
{
	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (playerBullets[i].visible && (playerBullets[i].row - 1) < 0)
		{
			playerBullets[i].visible = false;
		}
		else if (playerBullets[i].visible)
		{
			playerBullets[i].row = playerBullets[i].row - 1;
		}
	}
}

static void CreatePlayerBullet()
{
	// test용으로 장난침. 총알 인덱스 체크 해야함.
	//for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	//{
	//	if (!playerBullets[i].visible)
	//	{
	//		playerBullets[i].visible = true;
	//		playerBullets[i].row = player.row - 1;
	//		playerBullets[i].col = player.col - 3;
	//		playerBullets[i].damage = 100;

	//		break;
	//	}
	//}

	//for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	//{
	//	if (!playerBullets[i].visible)
	//	{
	//		playerBullets[i].visible = true;
	//		playerBullets[i].row = player.row - 1;
	//		playerBullets[i].col = player.col + 3;
	//		playerBullets[i].damage = 100;

	//		break;
	//	}
	//}

	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (!playerBullets[i].visible)
		{
			playerBullets[i].visible = true;
			playerBullets[i].row = player.row - 1;
			playerBullets[i].col = player.col;
			playerBullets[i].damage = 100;

			break;
		}
	}
}

static void PlayerBulletCollision()
{
	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (playerBullets[i].visible)
		{
			for (int j = 0; j < MAX_ENEMY_COUNT; j++)
			{
				if (enemys[j].visible && playerBullets[i].row == enemys[j].row && playerBullets[i].col == enemys[j].col)
				{
					enemys[j].hp -= playerBullets[i].damage;

					playerBullets[i].visible = false;

					if (enemys[j].hp <= 0)
						enemys[j].visible = false;

					return;
				}
			}
		}
	}
}

static void MoveEnemyBullet()
{
	for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
	{
		if (enemyBullets[i].visible && (enemyBullets[i].row + 1) >= SCREEN_ROW)
		{
			enemyBullets[i].visible = false;
		}
		else if (enemyBullets[i].visible)
		{
			enemyBullets[i].row = enemyBullets[i].row + 1;
		}
	}
}

static void CreateEnemyBullet()
{
	for (int j = 0; j < MAX_ENEMY_COUNT; j++)
	{
		if (enemys[j].visible)
		{
			for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
			{
				if (!enemyBullets[i].visible)
				{
					enemyBullets[i].visible = true;
					enemyBullets[i].row = enemys[j].row + 1;
					enemyBullets[i].col = enemys[j].col;
					enemyBullets[i].damage = 30;

					break;
				}
			}
		}
	}
}

static void EnemyBulletCollision()
{
	for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
	{
		if (enemyBullets[i].visible && enemyBullets[i].row == player.row && enemyBullets[i].col == player.col)
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