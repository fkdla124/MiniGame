#include <windows.h>
#include "Render.h"
#include "Scene.h"
#include "Data.h"

static void TitleSceneInput();
static void TitleSceneLogic();
static void TitleSceneRender();

extern bool renderSkip;
extern int logicCount;
extern int renderCount;

void TitleScene()
{
	TitleSceneInput();

	TitleSceneLogic();

	TitleSceneRender();
}

static void TitleSceneInput()
{
	if (GetAsyncKeyState('Q'))
	{
		scene = EXIT_SCENE;
	}

	if (GetAsyncKeyState('1') & 0x0001)
	{
		currentStage = 0;
		scene = LOAD_SCENE;
	}
}

static void TitleSceneLogic()
{
	logicCount++;
}

static void TitleSceneRender()
{
	if (renderSkip)
	{
		renderSkip = false;
		return;
	}

	renderCount++;

	ClearGameScreen();

	DrawTextGameScreen(SCREEN_ROW / 3, SCREEN_COL / 3, "[ Mini Shooting Game ]");
	DrawTextGameScreen(SCREEN_ROW / 3 + 2, SCREEN_COL / 3, "Game Start : 1");
	DrawTextGameScreen(SCREEN_ROW / 3 + 4, SCREEN_COL / 3, "Quit : q");

	DrawGameScreen();
}