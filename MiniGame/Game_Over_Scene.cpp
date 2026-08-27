#include <windows.h>
#include "Render.h"
#include "Scene.h"
#include "Data.h"

extern bool renderSkip;
extern int logicCount;
extern int renderCount;

static void GameOverSceneInput();
static void GameOverSceneLogic();
static void GameOverSceneRender();

void GameOverScene()
{
	GameOverSceneInput();

	GameOverSceneLogic();

	GameOverSceneRender();
}

static void GameOverSceneInput()
{
	if (GetAsyncKeyState('Q'))
	{
		scene = EXIT_SCENE;
	}

	// 한번의 입력으로 여러번 후루룩 넘어가는거 방지
	if (GetAsyncKeyState('1') & 0x0001 && GetAsyncKeyState('1') & 0x8000)
	{
		scene = TITLE_SCENE;
	}

	if (GetAsyncKeyState('2'))
	{
		currentStage = 0;
		scene = LOAD_SCENE;
	}
}

static void GameOverSceneLogic()
{
	logicCount++;
}

static void GameOverSceneRender()
{
	if (renderSkip)
	{
		renderSkip = false;
		return;
	}

	renderCount++;

	ClearGameScreen();

	DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "=== GAME OVER... ===");
	DrawTextGameScreen(SCREEN_ROW / 3 + 2, SCREEN_COL / 3, "Back Title : 1");
	DrawTextGameScreen(SCREEN_ROW / 3 + 4, SCREEN_COL / 3, "Restart Game : 2");
	DrawTextGameScreen(SCREEN_ROW / 3 + 6, SCREEN_COL / 3, "Quit : q");

	DrawGameScreen();
}