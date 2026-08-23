#include "Game.h"
#include <cmath>

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
static void CreateEnemyBullet(Enemy* enemy);
static void EnemyBulletCollision();

static void CreateEnemy(int row, int col);

LARGE_INTEGER tick;
LARGE_INTEGER freq;

static __int64 prevTime;
static float deltaTime;
static bool firstFrame = true;

// 테스트용. 지워야됨. 타이틀이나 이런데서 키 입력하는거 쿨타임 주려고 만듬
static float testTimer;

void GameScene()
{
	GameSceneInput();

	GameSceneLogic();

	GameSceneRender();
}

static void GameSceneInput()
{
	if ((player.moveTimer >= (1.0f / player.moveSpeed)))
	{
		if (GetAsyncKeyState(VK_LEFT))
		{
			//MoveConsoleCursor(0, 30);
			//static int countmove = 0;
			//countmove++;
			//std::cout << player.moveTimer << " " << countmove << " " << deltaTime;
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

	if (GetAsyncKeyState('Z'))
	{
		CreatePlayerBullet();
	}

	// The World!!!
	if (GetAsyncKeyState('C'))
	{
		memset(enemyBullets, 0, sizeof(enemyBullets));
	}

	// ALL KILL!!!
	if (GetAsyncKeyState('X'))
	{
		if (testTimer >= 0.1f)
		{
			memset(enemyBullets, 0, sizeof(enemyBullets));
			memset(enemys, 0, sizeof(enemys));
			testTimer = 0;
		}
	}
}

static void GameSceneLogic()
{
	if (firstFrame)
	{
		QueryPerformanceFrequency(&freq);

		QueryPerformanceCounter(&tick);
		prevTime = tick.QuadPart;
		firstFrame = false;
	}
	QueryPerformanceCounter(&tick);
	__int64 currentTime = tick.QuadPart;
	deltaTime = static_cast<float>(currentTime - prevTime) / freq.QuadPart;
	prevTime = currentTime;

	// updatePlayer() ?
	player.shotTimer += deltaTime;
	player.moveTimer += deltaTime;

	// updatePlayerBullet() ?
	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (playerBullets[i].visible)
		{
			playerBullets[i].moveTimer += deltaTime;
		}
	}

	// updateEnemy() ?
	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
	{
		if (enemys[i].visible)
		{
			enemys[i].moveTimer += deltaTime;
			enemys[i].shotTimer += deltaTime;
		}
	}


	// updateEnemyBullet() ?
	for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
	{
		if (enemyBullets[i].visible)
		{
			enemyBullets[i].moveTimer += deltaTime;
		}
	}

	testTimer += deltaTime;

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

//static void CreateEnemy(int row, int col)
//{
//	if (row < 0 || row >= SCREEN_ROW || col < 0 || col >= SCREEN_COL)
//		return;
//
//	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
//	{
//		if (!enemys[i].visible)
//		{
//			enemys[i].visible = true;
//			enemys[i].row = row;
//			enemys[i].col = col;
//			enemys[i].hp = 100;
//
//			return;
//		}
//	}
//}
//
//// test!!
//void TestEnemySet()
//{
//	for (int i = 0; i < 10; i++)
//	{
//		CreateEnemy(5, 20 + i * 5);
//	}
//	for (int i = 0; i < 10; i++)
//	{
//		CreateEnemy(10, 20 + i * 5);
//	}
//}

static void EnemyAI()
{
	// move logic
	for (int i = 0; i < MAX_ENEMY_COUNT; i++)
	{
		if (enemys[i].visible)
		{
			switch (enemys[i].moveType)
			{
				case MoveType::STOP:
					break;
				case MoveType::LEFT_RIGHT:
					if (enemys[i].moveDirection == 0) // left
					{
						// 좌표가 화면밖에 나가도, 렌더링할때 좌표를 검사해서 렌더링 하기때문에 괜찮다고 판단.
						// enemys[i].col = max(enemys[i].col - 1, 0);
						if (enemys[i].moveTimer >= (1.0f / enemys[i].moveSpeed))
						{
							enemys[i].moveTimer = 0;
							enemys[i].col -= 1;

							// 여기 있어야됨. 방향전환이 칸 이동과 동시에 한번만 일어나게.
							if (abs(enemys[i].col - enemys[i].spawnCol) >= enemys[i].moveRange)
							{
								enemys[i].moveDirection ^= 1;
							}
						}
					}
					else if (enemys[i].moveDirection == 1) // right
					{
						// enemys[i].col = min(enemys[i].col + 1, SCREEN_COL - 2);
						if (enemys[i].moveTimer >= (1.0f / enemys[i].moveSpeed))
						{
							enemys[i].moveTimer = 0;
							enemys[i].col += 1;

							// 여기 있어야됨. 방향전환이 칸 이동과 동시에 한번만 일어나게.
							if (abs(enemys[i].col - enemys[i].spawnCol) >= enemys[i].moveRange)
							{
								enemys[i].moveDirection ^= 1;
							}
						}
					}
					break;
			}

			// shot logic
			if (enemys[i].shotTimer >= enemys[i].shotCooldown)
			{
				enemys[i].shotTimer = 0;
				CreateEnemyBullet(&enemys[i]);
			}
		}
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
			if (playerBullets[i].moveTimer >= (1.0f / playerBullets[i].moveSpeed))
			{
				playerBullets[i].moveTimer = 0;
				playerBullets[i].row -= 1;
			}
		}
	}
}

static void CreatePlayerBullet()
{
	// test용으로 장난침. 총알 인덱스 체크 해야함.
	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (!playerBullets[i].visible && player.shotTimer >= player.shotCooldown)
		{
			playerBullets[i].visible = true;
			playerBullets[i].row = player.row - 1;
			playerBullets[i].col = player.col;
			playerBullets[i].damage = player.attack;
			playerBullets[i].moveSpeed = player.bulletSpeed;

			break;
		}
	}

	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (!playerBullets[i].visible && player.shotTimer >= player.shotCooldown)
		{
			playerBullets[i].visible = true;
			playerBullets[i].row = player.row - 1;
			playerBullets[i].col = player.col - 1;
			playerBullets[i].damage = player.attack;
			playerBullets[i].moveSpeed = player.bulletSpeed;

			break;
		}
	}

	for (int i = 0; i < MAX_PLAYER_BULLET_COUNT; i++)
	{
		if (!playerBullets[i].visible && player.shotTimer >= player.shotCooldown)
		{
			playerBullets[i].visible = true;
			playerBullets[i].row = player.row - 1;
			playerBullets[i].col = player.col + 1;
			playerBullets[i].damage = player.attack;
			playerBullets[i].moveSpeed = player.bulletSpeed;

			player.shotTimer = 0.0f;

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
				int playerBulletRow = playerBullets[i].row;
				int playerBulletCol = playerBullets[i].col;
				int enemyRow = enemys[j].row;
				int enemyCol = enemys[j].col;
				if (enemys[j].visible && playerBulletRow == enemyRow && playerBulletCol == enemyCol)
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
		if (enemyBullets[i].visible && (enemyBullets[i].col < 0 || enemyBullets[i].row < 0 || enemyBullets[i].row >= SCREEN_ROW || enemyBullets[i].col >= (SCREEN_COL - 1)))
		{
			enemyBullets[i].visible = false;
		}
		else if (enemyBullets[i].visible && enemyBullets[i].moveTimer >= (1.0f / enemyBullets[i].moveSpeed))
		{
			switch (enemyBullets[i].direction)
			{
				case BulletDirection::LEFT:
					enemyBullets[i].col -= 1;
					break;
				case BulletDirection::LEFT_UP:
					enemyBullets[i].col -= 1;
					enemyBullets[i].row -= 1;
					break;
				case BulletDirection::UP:
					enemyBullets[i].row -= 1;
					break;
				case BulletDirection::RIGHT_UP:
					enemyBullets[i].col += 1;
					enemyBullets[i].row -= 1;
					break;
				case BulletDirection::RIGHT:
					enemyBullets[i].col += 1;
					break;
				case BulletDirection::RIGHT_DOWN:
					enemyBullets[i].col += 1;
					enemyBullets[i].row += 1;
					break;
				case BulletDirection::DOWN:
					enemyBullets[i].row += 1;
					break;
				case BulletDirection::LEFT_DOWN:
					enemyBullets[i].col -= 1;
					enemyBullets[i].row += 1;
					break;
			}

			enemyBullets[i].moveTimer = 0;
		}
	}
}

static void CreateEnemyBullet(Enemy* enemy)
{
	//for (int j = 0; j < MAX_ENEMY_COUNT; j++)
	//{
	//	if (enemy->visible)
	//	{
			switch (enemy->shotType)
			{
				case ShotType::DOWN:
					for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
					{
						if (!enemyBullets[i].visible)
						{
							enemyBullets[i].visible = true;
							enemyBullets[i].row = enemy->row + 1;
							enemyBullets[i].col = enemy->col;
							enemyBullets[i].damage = enemy->attack;
							enemyBullets[i].moveSpeed = enemy->bulletSpeed;
							enemyBullets[i].direction = BulletDirection::DOWN;

							break;
						}
					}
					break;
				case ShotType::FOUR_WAY:
					for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
					{
						if (!enemyBullets[i].visible)
						{
							enemyBullets[i].visible = true;
							enemyBullets[i].row = enemy->row;
							enemyBullets[i].col = enemy->col - 1;
							enemyBullets[i].damage = enemy->attack;
							enemyBullets[i].moveSpeed = enemy->bulletSpeed;
							enemyBullets[i].direction = BulletDirection::LEFT;

							break;
						}
					}
					for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
					{
						if (!enemyBullets[i].visible)
						{
							enemyBullets[i].visible = true;
							enemyBullets[i].row = enemy->row - 1;
							enemyBullets[i].col = enemy->col;
							enemyBullets[i].damage = enemy->attack;
							enemyBullets[i].moveSpeed = enemy->bulletSpeed;
							enemyBullets[i].direction = BulletDirection::UP;

							break;
						}
					}
					for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
					{
						if (!enemyBullets[i].visible)
						{
							enemyBullets[i].visible = true;
							enemyBullets[i].row = enemy->row;
							enemyBullets[i].col = enemy->col + 1;
							enemyBullets[i].damage = enemy->attack;
							enemyBullets[i].moveSpeed = enemy->bulletSpeed;
							enemyBullets[i].direction = BulletDirection::RIGHT;

							break;
						}
					}
					for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
					{
						if (!enemyBullets[i].visible)
						{
							enemyBullets[i].visible = true;
							enemyBullets[i].row = enemy->row + 1;
							enemyBullets[i].col = enemy->col;
							enemyBullets[i].damage = enemy->attack;
							enemyBullets[i].moveSpeed = enemy->bulletSpeed;
							enemyBullets[i].direction = BulletDirection::DOWN;

							break;
						}
					}
					break;
				case ShotType::EIGHT_WAY:
					for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
					{
						if (!enemyBullets[i].visible)
						{
							enemyBullets[i].visible = true;
							enemyBullets[i].row = enemy->row;
							enemyBullets[i].col = enemy->col - 1;
							enemyBullets[i].damage = enemy->attack;
							enemyBullets[i].moveSpeed = enemy->bulletSpeed;
							enemyBullets[i].direction = BulletDirection::LEFT;

							break;
						}
					}
					for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
					{
						if (!enemyBullets[i].visible)
						{
							enemyBullets[i].visible = true;
							enemyBullets[i].row = enemy->row - 1;
							enemyBullets[i].col = enemy->col - 1;
							enemyBullets[i].damage = enemy->attack;
							enemyBullets[i].moveSpeed = enemy->bulletSpeed;
							enemyBullets[i].direction = BulletDirection::LEFT_UP;

							break;
						}
					}
					for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
					{
						if (!enemyBullets[i].visible)
						{
							enemyBullets[i].visible = true;
							enemyBullets[i].row = enemy->row - 1;
							enemyBullets[i].col = enemy->col;
							enemyBullets[i].damage = enemy->attack;
							enemyBullets[i].moveSpeed = enemy->bulletSpeed;
							enemyBullets[i].direction = BulletDirection::UP;

							break;
						}
					}
					for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
					{
						if (!enemyBullets[i].visible)
						{
							enemyBullets[i].visible = true;
							enemyBullets[i].row = enemy->row - 1;
							enemyBullets[i].col = enemy->col + 1;
							enemyBullets[i].damage = enemy->attack;
							enemyBullets[i].moveSpeed = enemy->bulletSpeed;
							enemyBullets[i].direction = BulletDirection::RIGHT_UP;

							break;
						}
					}
					for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
					{
						if (!enemyBullets[i].visible)
						{
							enemyBullets[i].visible = true;
							enemyBullets[i].row = enemy->row;
							enemyBullets[i].col = enemy->col + 1;
							enemyBullets[i].damage = enemy->attack;
							enemyBullets[i].moveSpeed = enemy->bulletSpeed;
							enemyBullets[i].direction = BulletDirection::RIGHT;

							break;
						}
					}
					for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
					{
						if (!enemyBullets[i].visible)
						{
							enemyBullets[i].visible = true;
							enemyBullets[i].row = enemy->row + 1;
							enemyBullets[i].col = enemy->col + 1;
							enemyBullets[i].damage = enemy->attack;
							enemyBullets[i].moveSpeed = enemy->bulletSpeed;
							enemyBullets[i].direction = BulletDirection::RIGHT_DOWN;

							break;
						}
					}
					for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
					{
						if (!enemyBullets[i].visible)
						{
							enemyBullets[i].visible = true;
							enemyBullets[i].row = enemy->row + 1;
							enemyBullets[i].col = enemy->col;
							enemyBullets[i].damage = enemy->attack;
							enemyBullets[i].moveSpeed = enemy->bulletSpeed;
							enemyBullets[i].direction = BulletDirection::DOWN;

							break;
						}
					}
					for (int i = 0; i < MAX_ENEMY_BULLET_COUNT; i++)
					{
						if (!enemyBullets[i].visible)
						{
							enemyBullets[i].visible = true;
							enemyBullets[i].row = enemy->row + 1;
							enemyBullets[i].col = enemy->col - 1;
							enemyBullets[i].damage = enemy->attack;
							enemyBullets[i].moveSpeed = enemy->bulletSpeed;
							enemyBullets[i].direction = BulletDirection::LEFT_DOWN;

							break;
						}
					}
					break;
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