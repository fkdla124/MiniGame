#include "Game.h"

Scene scene = TITLE_SCENE;
int currentStage = 0;

int fps = 0;
unsigned long fpsTick = timeGetTime();
static void countFPS()
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

unsigned long mainTick = timeGetTime();
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

		unsigned long diff = timeGetTime() - mainTick;
		mainTick += 20;

		countFPS();

		// 50 fps
		if ((20 - diff) < 20)
			Sleep(20 - diff);
	}

	// todo
	// [V] [우선순위 높음] 모든 오브젝트의 이동 로직 프레임 의존 으로 구현.
	//                 ㄴ-> ex. 2프레임에 1칸 움직여요. -> 1초에 25칸 움직여요. moveSpeed = 25 입니다~
	// 
	// [X] [우선순위 높음] 파일 로드 과정 예외처리 구현. 버퍼 활용도 좀 깔끔하게. new 활용할거면 잘좀 생각해서 해보자.
	//
	// [X] [우선순위 높음] enemy파일 로드시 moveType, moveRange 삭제. 이제 필요없어짐.
 	// 
	// [X] [우선순위 높음] 메뉴씬 1번 키 쿨타임 추가하기.
	// 
	// [X] [우선순위 높음] 파일을 너무 많이 나눴다. 구조를 좀 정리해야한다. 한 cpp 파일에서만 쓰는 데이터는 
	//                    Data.h에 있을 필요가 없다. 아니 애초에 Data.h 파일이 있어야 할까? 잘 모르겠다..
	// 
	// [X] [우선순위 높음] 현재 movePattern에 'L' 'R' 'U' 'D' 를 코드상에서 구분하는데.
	//                    파일에서 'L'에 대한 정의를 해야함.
	//					  예시로 파일에
	// 
	//                    move_pattern1.txt
	//                    {
	//                        L 0 -1
	//                        R 0 1
	//                        LLLRRRRRRLLL
	//                    }
	//                    
	//                    그리고 코드상에서 그것을 실행만 해야함. 근데 이거 구현할 시간이 없다....
	// 
	// 
	// 
	// 
	// 
	// 

	return 0;
}