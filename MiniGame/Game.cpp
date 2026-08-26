#include "Game.h"
#include <conio.h>

Scene scene = TITLE_SCENE;
int currentStage = 0;

// Render쪽에서 뽑아쓰는 fps
int fps;

// fps 계산
static unsigned long fpsTick = timeGetTime();
static void CountFPS();

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

		CountFPS();

		LimitFPS();
	}

	return 0;
}

static void CountFPS()
{
	static int count = 0;
	count++;

	unsigned long diff = timeGetTime() - fpsTick;
	if (diff >= 1000)
	{
		fpsTick += 1000;
		fps = count;
		count = 0;
	}
}

static void LimitFPS()
{
	unsigned long diff = timeGetTime() - mainTick;
	mainTick += 20;

	// 50 fps
	if ((20 - diff) < 20)
		Sleep(20 - diff);
}