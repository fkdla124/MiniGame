#include <windows.h>
#include "Data.h"
#include "Render.h"
#include "Logic.h"

void Input()
{
	if (GetAsyncKeyState(VK_LEFT))
	{
		player.col = max(player.col - 1, 0);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		player.col = min(player.col + 1, SCREEN_COL - 2);
	}
	if (GetAsyncKeyState(VK_UP))
	{
		player.row = max(player.row - 1, 0);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		player.row = min(player.row + 1, SCREEN_ROW - 1);
	}
	if (GetAsyncKeyState(VK_LCONTROL))
	{
		CreatePlayerBullet();
	}

}