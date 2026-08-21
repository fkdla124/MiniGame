#include "Game.h"

int main()
{
	InitConsoleCursor();

	timeBeginPeriod(1);

	TestEnemySet();

	while (1)
	{
		Input();

		Logic();

		Render();

		Sleep(30);
	}

	return 0;
}