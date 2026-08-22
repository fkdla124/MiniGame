#include "Game.h"

Scene scene = TITLE_SCENE;
int currentStage = 0;

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
				return 0;
		}
		Sleep(70);
	}

	return 0;
}