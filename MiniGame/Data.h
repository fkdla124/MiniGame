#pragma once
#include "Render.h"


#define MAX_ENEMY_COUNT 100
#define MAX_PLAYER_BULLET_COUNT 30
#define MAX_ENEMY_BULLET_COUNT 300

struct Player
{
	bool visible;

	int hp;
	int attack;
	int moveSpeed;
	float moveTimer;
	int bulletSpeed;
	float shotCooldown;
	float shotTimer;

	int row;
	int col;
};

enum class MoveType
{
	STOP = 1,
	LEFT_RIGHT = 2,
};

enum class ShotType
{
	DOWN = 1,
	FOUR_WAY = 2,
	EIGHT_WAY = 3
};

struct Enemy
{
	bool visible;

	MoveType moveType;
	ShotType shotType;

	int moveRange;
	int moveDirection;

	int hp;
	int attack;
	int moveSpeed;
	float moveTimer;
	int bulletSpeed;
	float shotCooldown;
	float shotTimer;

	int spawnRow;
	int spawnCol;
	int row;
	int col;
};

struct PlayerBullet
{
	bool visible;

	int row;
	int col;

	int moveSpeed;
	float moveTimer;
	int damage;
};

enum class BulletDirection
{
	LEFT = 0,
	LEFT_UP = 1,
	UP = 2,
	RIGHT_UP = 3,
	RIGHT = 4,
	RIGHT_DOWN = 5,
	DOWN = 6,
	LEFT_DOWN = 7,
};

struct EnemyBullet
{
	bool visible;

	int row;
	int col;

	int moveSpeed;
	float moveTimer;
	int damage;
	BulletDirection direction;
};

struct Stage
{
	char screen[SCREEN_ROW][SCREEN_COL];
};

enum Scene
{
	GAME_SCENE = 0,
	GAME_OVER_SCENE = 1,
	GAME_CLEAR_SCENE = 2,
	LOAD_SCENE = 3,
	TITLE_SCENE = 4,

	EXIT_SCENE = -1
};

extern Scene scene;
extern int currentStage;
extern int stageCount;

extern Player player;
extern Enemy enemys[MAX_ENEMY_COUNT];
extern PlayerBullet playerBullets[MAX_PLAYER_BULLET_COUNT];
extern EnemyBullet enemyBullets[MAX_ENEMY_BULLET_COUNT];