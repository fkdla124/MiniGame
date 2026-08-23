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
		Sleep(3);
	}

	// todo
	// 델타타임을 전역적으로 사용 가능하게 분리?
	// 타이틀에서 키 입력시 프레임이 너무 빨라서 여러번 입력됨. (쿨타임 만들기)
	// 플레이어 HP 표시.
	// 하고싶으면 아이템도 만들어보기?
	// 파일 로딩 깔끔하게 오류없게 수정하기.
	// 총알, 적 움직이는 것들 인덱스 체크 똑바로 하기.
	// 구조 깔끔하게 변경. 코드 읽기 쉽게 변경.

	return 0;
}