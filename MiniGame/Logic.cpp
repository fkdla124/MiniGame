#include "Game.h"

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

static void PlayerBulletMove()
{
	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (playerBullets[i].visible)
		{
			if ((playerBullets[i].row - 1) < 0)
			{
				playerBullets[i].visible = false;
			}

			playerBullets[i].row = playerBullets[i].row - 1;
		}
	}
}

void CreatePlayerBullet()
{
	// test용으로 장난침. 총알 인덱스 체크 해야함.
	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (!playerBullets[i].visible)
		{
			playerBullets[i].visible = true;
			playerBullets[i].row = player.row - 1;
			playerBullets[i].col = player.col - 3;
			playerBullets[i].damage = 100;

			break;
		}
	}

	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (!playerBullets[i].visible)
		{
			playerBullets[i].visible = true;
			playerBullets[i].row = player.row - 1;
			playerBullets[i].col = player.col + 3;
			playerBullets[i].damage = 100;

			break;
		}
	}

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

void Logic()
{
	EnemyAI();
	PlayerBulletMove();
	PlayerBulletCollision();
}