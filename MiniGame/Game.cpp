#include "Game.h"
#include <conio.h>

Scene scene = TITLE_SCENE;
int currentStage = 0;

// Logic 수행 횟수
int logicCount;

// Render 수행 횟수
int renderCount;

// Render 스킵 여부
bool renderSkip = false;

// Render 에서 뽑아쓰는 fps
int logicFPS;
int renderFPS;

// fps 계산
static unsigned long fpsTick = timeGetTime();
static void UpdateFPS();

// fps 제한
static unsigned long mainTick = timeGetTime();
static void LimitFPS();

int main()
{
	InitConsoleCursor();
	timeBeginPeriod(1);

	while (1)
	{
		switch (scene)
		{
			case GAME_SCENE:
				GameScene();
				break;

			case GAME_OVER_SCENE:
				GameOverScene();
				break;

			case GAME_CLEAR_SCENE:
				GameClearScene();
				break;

			case LOAD_SCENE:
				LoadScene();
				break;

			case TITLE_SCENE:
				TitleScene();
				break;

			case EXIT_SCENE:
				// 뭔가 입력시 프로그램 종료. (오류 출력 보여줘야함)
				while (_kbhit())
					int c = _getch();

				int c = _getch();
				return 0;
		}

		UpdateFPS();

		LimitFPS();
	}

	return 0;
}

static void UpdateFPS()
{
	unsigned long diff = timeGetTime() - fpsTick;

	if (diff >= 1000)
	{
		fpsTick += 1000;

		logicFPS = logicCount;
		renderFPS = renderCount;
		logicCount = 0;
		renderCount = 0;
	}
}

static void LimitFPS()
{
	unsigned long diff = timeGetTime() - mainTick;
	mainTick += 20;

	// Render Skip
	if (diff > 39)
	{
		renderSkip = true;
		return;
	}

	// 50 fps
	if ((20 - diff) < 20)
		Sleep(20 - diff);
}