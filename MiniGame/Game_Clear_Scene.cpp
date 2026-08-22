#include <windows.h>
#include "Render.h"
#include "Scene.h"
#include "Data.h"

static void GameClearSceneInput();
static void GameClearSceneLogic();
static void GameClearSceneRender();

void GameClearScene()
{
	GameClearSceneInput();

	GameClearSceneLogic();

	GameClearSceneRender();
}

static void GameClearSceneInput()
{
	if (GetAsyncKeyState('Q'))
	{
		scene = EXIT_SCENE;
	}

	if (GetAsyncKeyState('1'))
	{
		scene = TITLE_SCENE;
	}

	if (GetAsyncKeyState('2'))
	{
		currentStage = 0;
		scene = LOAD_SCENE;
	}
}

static void GameClearSceneLogic()
{

}

static void GameClearSceneRender()
{
	ClearGameScreen();

	DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "=== GAME CLEAR!!! ===");
	DrawTextGameScreen(SCREEN_ROW / 3 + 2, SCREEN_COL / 3, "Back Title : 1");
	DrawTextGameScreen(SCREEN_ROW / 3 + 4, SCREEN_COL / 3, "Restart Game : 2");
	DrawTextGameScreen(SCREEN_ROW / 3 + 6, SCREEN_COL / 3, "Quit : q");

	DrawGameScreen();
}