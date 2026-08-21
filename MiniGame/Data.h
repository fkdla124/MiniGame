#pragma once

#define MAX_ENEMY_COUNT 100
#define MAX_PLAYER_BULLET_COUNT 100
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

extern Player player;
extern Enemy enemys[MAX_ENEMY_COUNT];
extern PlayerBullet playerBullets[MAX_PLAYER_BULLET_COUNT];
extern EnemyBullet enemyBullets[MAX_ENEMY_BULLET_COUNT];