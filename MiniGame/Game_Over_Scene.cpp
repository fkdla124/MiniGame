#include <windows.h>
#include "Render.h"
#include "Scene.h"
#include "Data.h"

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

	if (GetAsyncKeyState('1') & 0x0001)
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

}

static void GameOverSceneRender()
{
	ClearGameScreen();

	DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "=== GAME OVER... ===");
	DrawTextGameScreen(SCREEN_ROW / 3 + 2, SCREEN_COL / 3, "Back Title : 1");
	DrawTextGameScreen(SCREEN_ROW / 3 + 4, SCREEN_COL / 3, "Restart Game : 2");
	DrawTextGameScreen(SCREEN_ROW / 3 + 6, SCREEN_COL / 3, "Quit : q");

	DrawGameScreen();
}