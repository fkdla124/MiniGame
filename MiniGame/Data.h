#pragma once
#include "Render.h"


#define MAX_ENEMY_COUNT 100
#define MAX_PLAYER_BULLET_COUNT 10
#define MAX_ENEMY_BULLET_COUNT 100

struct Player
{
	bool visible;

	int hp;

	int row;
	int col;
};

struct Enemy
{
	bool visible;

	int hp;

	int row;
	int col;
};

struct PlayerBullet
{
	bool visible;

	int row;
	int col;

	int damage;
};

struct EnemyBullet
{
	bool visible;

	int row;
	int col;

	int damage;
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