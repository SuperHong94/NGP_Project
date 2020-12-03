#pragma comment(lib, "ws2_32")
#include "global.h"

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Window Programming Lab";
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
EROUND eRound = MAIN;
bool PauseOnOff = true;
using namespace std;

//#define SERVERIP "192.168.219.100"
//#define SERVERIP "192.168.219.103"
//#define SERVERIP "192.168.43.181"
//#define SERVERIP "192.168.43.167"
#define SERVERIP "192.168.219.102"
//#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 5000

vector<int> v;
int acceptClientCnt = 0;
int clientCnt = 0;

bool multiSelect = false;

// 오류 출력 함수
void err_quit(char* msg);
void err_display(char* msg);

// 소켓 통신 스레드 함수
int CreateSocket();
SOCKET UDPsock;
SOCKET TCPsock;

int multiStart = 0;
bool multiOn = false;
SOCKADDR_IN Tserveraddr, Userveraddr;
int sendTcpData(tcpData playerData);
int sendUdpData(udpData playerData);
void recvTcpData();
void recvUdpData();
void UpdateData();
//udpData *p1UdpData = new udpData[1];
//tcpData *p1TcpData = new tcpData[1];
//udpData* p2UdpData = new udpData[1];
//tcpData* p2TcpData = new tcpData[1];

udpData p1UdpData;
tcpData p1TcpData;
udpData p2UdpData;
tcpData p2TcpData;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	//윈도우 클래스 구조체 값 설정
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(RGB(0, 255, 0));
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	//윈도우 클래스 등록
	RegisterClassEx(&WndClass);

	//윈도우 생성
	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 1200, 800, NULL, (HMENU)NULL, hInstance, NULL);
	//int cnt = 0
	//	1
	p1UdpData = { 'u', '1' };
	p1TcpData = { 't', '1' , false, 0, 0,390,390,false, 100 ,5,5, EROUND::MAIN };


	p2UdpData = { 'u', '2'};
	p2TcpData = { 't', '2' , false, 0, 0,390,390,false, 100 ,5,5,  EROUND::MAIN };

	char type;
	char playerID;
	bool useTeleport;
	int teleportXpos;
	int teleportYpos;
	bool useDash;
	int hp;

	//윈도우 출력
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//이벤트 루프 처리
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);

	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static bool menuOnOff = true; //메뉴 그릴지 말지

	static HDC hDC, MemDC;
	static HBITMAP BackBit, oldBackBit, hBit[10];
	static RECT bufferRT;
	PAINTSTRUCT ps;
	static Boom* head;
	static Boom* bullet_head;
	static int syc = 0;

	char buffer[1000]; //초세는 타이머
	HFONT hFont;
	HBITMAP hBit1;
	BITMAP bit;

	//int boomCount = 0;  //폭탄 카운트는 0;

	//메시지 처리하기
	switch (uMsg) {
	case WM_CREATE:

		head = new Boom;
		bullet_head = new Boom;
		bullet_head->nextBoom = NULL;
		head->nextBoom = NULL;
		GetClientRect(hWnd, &WindowSize);
		GetClientRect(hWnd, &Energybar);
		GetClientRect(hWnd, &Energybar2);
		tmp.left = 0;
		tmp.right = 0;
		tmp.top = 0;
		tmp.bottom = 0;
		Tp = FALSE;
		Tp_2 = FALSE;
		Player_1.top = p1TcpData.playerYpos - 12.5;
		Player_1.bottom = p1TcpData.playerYpos + 12.5;
		Player_1.left = p1TcpData.playerXpos - 12.5;
		Player_1.right = p1TcpData.playerXpos + 12.5;
		PLAYER1_HIT = 0;

		Player_2.top = p2TcpData.playerYpos - 12.5;
		Player_2.bottom = p2TcpData.playerYpos + 12.5;
		Player_2.left = p2TcpData.playerXpos - 12.5;
		Player_2.right = p2TcpData.playerXpos + 12.5;
		PLAYER2_HIT = 0;

		Laser_Boom = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LASERBOOM));
		Circle_Boom = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_CIRCLEBOOM));
		Teleport = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_TELEPORT));
		Teleport2 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_TELEPORT2));
		PLAYER_1 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_PLAYER));
		PLAYER_2 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_PLAYER2));

		POWEROVERWHELMING = FALSE;

		soundSetup(); //사운드 셋업
		effSoundSetup();//이펙트 셋업


		SetTimer(hWnd, 0, 10, NULL);
		SetTimer(hWnd, 1, 100, NULL);
		SetTimer(hWnd, 2, 1, NULL);
		playSound(MainSound);
		break;
	case WM_LBUTTONDOWN:
		effPlaySound(click);
		switch (eRound)
		{
		case MAIN:
			ClickRange(lParam, eRound);
			break;
		case HELP:
			eRound = MAIN;
			break;
		case SelectPlay:
			multiStart = ClickRange(lParam, eRound);
			break;
		case Select:
			ClickRange(lParam, eRound);
			break;
		case Round1:
			cout << "a" << endl;
			break;
		case Round2:
			break;
		case YouDie:
			Energybar.right = WindowSize.right;
			Energybar2.right = WindowSize.right;
			p1TcpData.sceneState = MAIN;
			playSound(MainSound);
			eRound = MAIN;
			// closesocket()
			closesocket(TCPsock);
			// 윈속 종료
			WSACleanup();
			break;
		case YouWin:
			eRound = MAIN;
			Energybar.right = WindowSize.right;
			Energybar2.right = WindowSize.right;
			p1TcpData.sceneState = MAIN;
			playSound(MainSound);
			// closesocket()
			closesocket(TCPsock);
			// 윈속 종료
			WSACleanup();
			break;
		default:
			break;
		}
		break;
	case WM_TIMER:
		UpdateData();
		if (PauseOnOff)
		{
			switch (wParam) {
			case 2:
				
			if (multiStart == 1 && !multiOn)
			{
				CreateSocket();
				multiOn = true;
				multiStart++;
				p1TcpData.sceneState = MultiReady;
			}

			if (multiStart >= 1)
			{
				
				sendTcpData(p1TcpData);
				//sendUdpData(p1UdpData);
				
				recvTcpData();
				//recvUdpData();
	
			}
			break;
			case 0:
				PLAYER1DR = STOP;

				if (GetAsyncKeyState('A') < 0)
				{
					p1TcpData.playerXpos -= 3;

					if (p1TcpData.playerXpos < 12.5)
					{
						p1TcpData.playerXpos = 12.5;
					}
					PLAYER1DR = LEFT;
				}

				else if (GetAsyncKeyState('D') < 0)
				{
					p1TcpData.playerXpos += 3;

					if (p1TcpData.playerXpos > 1171.5)
					{
						p1TcpData.playerXpos = 1171.5;
					}
					PLAYER1DR = RIGHT;
				}

				if (GetAsyncKeyState('W') < 0)
				{
					p1TcpData.playerYpos -= 3;

					if (p1TcpData.playerYpos < 12.5)
					{
						p1TcpData.playerYpos = 12.5;
					}

					switch (PLAYER1DR)
					{
					case STOP:
						PLAYER1DR = UP;
						break;
					case LEFT:
						PLAYER1DR = UPLEFT;
						break;
					case RIGHT:
						PLAYER1DR = UPRIGHT;
						break;
					}
				}

				else if (GetAsyncKeyState('S') < 0)
				{
					p1TcpData.playerYpos += 3;

					switch (PLAYER1DR)
					{
					case STOP:
						PLAYER1DR = DOWN;
						break;
					case LEFT:
						PLAYER1DR = DOWNLEFT;
						break;
					case RIGHT:
						PLAYER1DR = DOWNRIGHT;
						break;
					}

					if (p1TcpData.playerYpos > WindowSize.bottom - 52.5)
					{
						p1TcpData.playerYpos = WindowSize.bottom - 52.5;
					}
				}

			/*	PLAYER2DR = STOP;

				if (GetAsyncKeyState('J') < 0)
				{
					Player_2.left -= 3;
					Player_2.right -= 3;
					if (Player_2.left < 0)
					{
						Player_2.left = 0;
						Player_2.right = 25;
					}
					PLAYER2DR = LEFT;
				}

				else if (GetAsyncKeyState('L') < 0)
				{
					Player_2.left += 3;
					Player_2.right += 3;

					if (Player_2.right > 1184)
					{
						Player_2.left = 1159;
						Player_2.right = 1184;
					}
					PLAYER2DR = RIGHT;
				}

				if (GetAsyncKeyState('I') < 0)
				{
					Player_2.top -= 3;
					Player_2.bottom -= 3;

					if (Player_2.top < 0)
					{
						Player_2.top = 0;
						Player_2.bottom = 25;
					}

					switch (PLAYER2DR)
					{
					case STOP:
						PLAYER2DR = UP;
						break;
					case LEFT:
						PLAYER2DR = UPLEFT;
						break;
					case RIGHT:
						PLAYER2DR = UPRIGHT;
						break;
					}
				}

				else if (GetAsyncKeyState('K') < 0)
				{
					Player_2.top += 3;
					Player_2.bottom += 3;
					switch (PLAYER2DR)
					{
					case STOP:
						PLAYER2DR = DOWN;
						break;
					case LEFT:
						PLAYER2DR = DOWNLEFT;
						break;
					case RIGHT:
						PLAYER2DR = DOWNRIGHT;
						break;
					}

					if (Player_2.bottom > WindowSize.bottom - 20)
					{
						Player_2.top = WindowSize.bottom - 45;
						Player_2.bottom = WindowSize.bottom - 20;
					}
				}*/


				CheckBullet(bullet_head);
				CheckBoom(head);
				setBoomPosition(bullet_head);
				setAnimation(head);
				setBoomPosition(head);

				if (!POWEROVERWHELMING)
				{
					CheckBulletCrush(bullet_head);
					CheckBoomCrush(head);
				}
				break;
			case 1:  //0.1초 단위로 생성됨
				sj_Timer++;
				if (Energybar.right <= 0) {
					eRound = YouDie;
					KillTimer(hWnd, 1);
					playSound(YOUDIE);
					menuOnOff = true;
					SetUp(head, bullet_head);
		
				}
				if (Energybar2.right <= 0) {
					eRound = YouWin;
					KillTimer(hWnd, 1);
					playSound(YOUWIN);
					menuOnOff = true;
					SetUp(head, bullet_head);
				}
				switch (eRound)
				{
				case Round1:
					switch (sj_Timer + syc) {
					case 10:
						SunBoom_SJ(hDC, bullet_head, 300, 300);
						break;
					case 20:
						SunBoom_SJ(hDC, bullet_head, 400, 400);
						break;
					case 30:
						SunBoom_SJ(hDC, bullet_head, 500, 500);
						break;
					case 40:
						SunBoom_SJ(hDC, bullet_head, 700, 700);
						break;
					case 45:
						SunBoom_SJ(hDC, bullet_head, 500, 500);
						break;
					case 50:
						SunBoom_SJ(hDC, bullet_head, 200, 200);
						break;
					case 55:
						SunBoom_SJ(hDC, bullet_head, 200, 300);
						break;
					case 60:
						SunBoom_SJ(hDC, bullet_head, 200, 300);
						break;
					case 70:
						SunBoom_SJ(hDC, bullet_head, 200, 400);
						break;
					case 75:
						SunBoom_SJ(hDC, bullet_head, 200, 500);
						break;
					case 76:
						SunBoom_SJ(hDC, bullet_head, 200, 600);
						break;
					case 100:
						SunBoom_SJ(hDC, bullet_head, 700, 100);
						break;
					case 125:
						SunBoom_SJ(hDC, bullet_head, 700, 200);
						break;
					case 130:
						SunBoom_SJ(hDC, bullet_head, 700, 200);
						break;
					case 140:
						SunBoom_SJ(hDC, bullet_head, 700, 300);
						break;
					case 145:
						SunBoom_SJ(hDC, bullet_head, 600, 100);
						break;
					case 146:
						SunBoom_SJ(hDC, bullet_head, 600, 700);
						break;
					case 147:
						SunBoom_SJ(hDC, bullet_head, 100, 400);
						break;
					case 148:
						SunBoom_SJ(hDC, bullet_head, 1100, 400);
						break;
					case 150:
						addBoom(head, Boom_Laser, 250, 250, 400, 400);
						break;
					case 152:
						addBoom(head, Boom_Laser, 800, 400, 650, 650);
						break;
					case 160:
						SunBoom_SJ(hDC, bullet_head, 700, 200);
						break;
					case 168:
						SunBoom_SJ(hDC, bullet_head, 700, 300);
						break;
					case 170:
						SunBoom_SJ(hDC, bullet_head, 700, 300);
						break;
					case 180:
						SunBoom_SJ(hDC, bullet_head, 500, 500);
						break;
					case 190:
						SunBoom_SJ(hDC, bullet_head, 300, 300);
						SunBoom_SJ(hDC, bullet_head, 700, 700);
						break;
					case 200:
						SunBoom_SJ(hDC, bullet_head, 100, 100);
						SunBoom_SJ(hDC, bullet_head, 900, 900);
						break;
					case 230:
						Doughnut(hDC, bullet_head, WindowSize.right / 2, WindowSize.bottom / 2, 100); //윈도우 중앙에서
						break;
					case 308:
						for (int i = 0; i < 6; i++)
							addBoom(head, Boom_Circle, i * 240, 550, 240 + i * 240, 790);
						break;
					case 310:
						for (int i = 0; i < 6; i++)
							addBoom(head, Boom_Circle, i * 240, 0, 240 + i * 240, 240);
						break;
					case 320:
						for (int i = 0; i < 6; i++)
							addBoom(head, Boom_Circle, i * 240, 220, 240 + i * 240, 480);
						break;
					case 380:
						for (int i = 0; i < 6; i++)
							addBoom(head, Boom_DownLaser, 0 + i * 240, -100, 120 + i * 240, 900);
						break;
					case 335:
						SunBoom_SJ(hDC, bullet_head, 120, 400);
						break;
					case 337:
						SunBoom_SJ(hDC, bullet_head, 360, 400);
						break;
					case 339:
						SunBoom_SJ(hDC, bullet_head, 480, 400);
						break;
					case 341:
						SunBoom_SJ(hDC, bullet_head, 600, 400);
						break;
					case 343:
						SunBoom_SJ(hDC, bullet_head, 840, 400);
						break;

					case 115:
						SunBoom_SJ(hDC, bullet_head, 600, 400);
						break;
					case 370:
						MeetBoom(hDC, bullet_head);
						break;
					case 415:
						SunBoom_SJ(hDC, bullet_head, 0, 0);
						break;
					case 417:
						SunBoom_SJ(hDC, bullet_head, 600, 0);
						break;
					case 419:
						SunBoom_SJ(hDC, bullet_head, 1100, 0);
						break;
					case 421:
						SunBoom_SJ(hDC, bullet_head, 0, 400);
						break;
					case 423:
						SunBoom_SJ(hDC, bullet_head, 600, 400);
						break;
					case 425:
						SunBoom_SJ(hDC, bullet_head, 1100, 400);
						break;
					case 427:
						SunBoom_SJ(hDC, bullet_head, 0, 700);
						break;
					case 429:
						SunBoom_SJ(hDC, bullet_head, 600, 700);
						break;
					case 431:
						SunBoom_SJ(hDC, bullet_head, 1100, 700);
						break;
					case 458:
						addBoom(head, Boom_LeftLaser, -100, 100, 1300, 300);
						break;
					case 460:
						addBoom(head, Boom_RightLaser, -100, 500, 1300, 700);
						break;

					case 465:
						addBoom(head, Boom_Circle, 460, 260, 660, 460);
						break;
					case 466:
						addBoom(head, Boom_Circle, 410, 210, 710, 510);
						break;
					case 467:
						addBoom(head, Boom_Circle, 360, 160, 760, 560);
						break;
					case 469:
						addBoom(head, Boom_Circle, 310, 110, 810, 610);
						break;
					case 470:
						addBoom(head, Boom_Circle, 260, 60, 860, 660);
						break;
					case 471:
						addBoom(head, Boom_Circle, 210, 10, 910, 710);
						break;
					case 472:
						addBoom(head, Boom_Circle, 160, -40, 960, 760);
						break;
					case 473:
						addBoom(head, Boom_Circle, 110, -90, 1010, 810);
						break;

					case 500://승리조건
						eRound = YouWin;
						score = sj_Timer * Energybar.right;
						playSound(YOUWIN);
						menuOnOff = true;
						break;
					}
			
					break;
				case Round2:
					switch (sj_Timer + syc)
					{
					case 10:
						addBoom(head, Boom_Circle, 100, 80, 500, 480);
						break;
					case 11:
						addBoom(head, Boom_Circle, 100, 350, 500, 750);
						break;
					case 12:
						addBoom(head, Boom_Circle, 500, 350, 800, 650);
						break;
					case 13:
						addBoom(head, Boom_Circle, 500, 80, 800, 380);
						break;
					case 40:
						addBoom(head, Boom_Circle, 450, 80, 1050, 680);
						break;
					case 41:
						addBoom(head, Boom_Circle, 50, 50, 350, 350);
						break;
					case 42:
						addBoom(head, Boom_Circle, 300, 300, 600, 600);
						break;
					case 110:
						addBoom(head, Boom_Laser, -100, 50, 1300, 100);
						break;
					case 111:
						addBoom(head, Boom_Laser, -100, 150, 1300, 200);
						break;
					case 112:
						addBoom(head, Boom_Laser, -100, 250, 1300, 300);
						break;
					case 113:
						addBoom(head, Boom_Laser, -100, 350, 1300, 400);
						break;
					case 114:
						addBoom(head, Boom_Laser, -100, 450, 1300, 500);
						break;
					case 115:
						addBoom(head, Boom_Laser, -100, 550, 1300, 600);
						break;
					case 116:
						addBoom(head, Boom_Laser, -100, 650, 1300, 700);
						break;
					case 117:
						addBoom(head, Boom_Laser, -100, 750, 1300, 800);
						break;
					case 160:
						addBoom(head, Boom_Laser, -100, 650, 1300, 700);
						break;
					case 161:
						addBoom(head, Boom_Laser, -100, 550, 1300, 600);
						break;
					case 162:
						addBoom(head, Boom_Laser, -100, 450, 1300, 500);
						break;
					case 163:
						addBoom(head, Boom_Laser, -100, 350, 1300, 400);
						break;
					case 164:
						addBoom(head, Boom_Laser, -100, 250, 1300, 300);
						break;
					case 165:
						addBoom(head, Boom_Laser, -100, 150, 1300, 200);
						break;
					case 166:
						addBoom(head, Boom_Laser, -100, 50, 1300, 100);
						break;
					case 203:
						SunBoom_SJ(hDC, bullet_head, 540, 380);
						break;
					case 208:
						SunBoom_SJ(hDC, bullet_head, 150, 200);
						break;
					case 209:
						SunBoom_SJ(hDC, bullet_head, 250, 300);
						break;
					case 210:
						SunBoom_SJ(hDC, bullet_head, 550, 300);
						break;
					case 240:
						addBoom(head, Boom_Rectangle, 50, 50, 350, 350);
						SunBoom_SJ(hDC, bullet_head, 750, 300);
						break;
					case 241:
						addBoom(head, Boom_Rectangle, 700, 150, 1000, 450);
						SunBoom_SJ(hDC, bullet_head, 650, 450);
						break;
					case 242:
						SunBoom_SJ(hDC, bullet_head, 730, 380);
						addBoom(head, Boom_Rectangle, 300, 300, 600, 600);
						break;
					case 243:
						addBoom(head, Boom_Rectangle, 850, 450, 1150, 750);

						break;
					case 296:
						Doughnut(hDC, head, 520, 380, 50);
						break;
					case 297:
						SunBoom_SJ(hDC, bullet_head, 100, 100);
						SunBoom_SJ(hDC, bullet_head, 1100, 100);
						SunBoom_SJ(hDC, bullet_head, 100, 700);
						SunBoom_SJ(hDC, bullet_head, 1100, 700);
						break;
					case 310:
						addBoom(head, Boom_LeftLaser, -100, 340, 1300, 390);
						addBoom(head, Boom_RightLaser, -100, 340, 1300, 390);
						addBoom(head, Boom_UpLaser, 540, -100, 590, 900);
						addBoom(head, Boom_DownLaser, 540, -100, 590, 900);
						addBoom(head, Boom_Laser2, 20, -100, 70, 900);
						SunBoom_SJ(hDC, bullet_head, 850, 650);
						break;
					case 315:
						addBoom(head, Boom_Laser2, 980, -100, 1030, 900);
						break;
					case 320:
						SunBoom_SJ(hDC, bullet_head, 250, 650);
						addBoom(head, Boom_Laser, -100, 120, 1300, 170);
						break;
					case 325:
						addBoom(head, Boom_Laser, -100, 600, 1300, 650);
						break;
					case 330:
						addBoom(head, Boom_Laser2, 150, -100, 200, 900);
						break;
					case 390:
						SunBoom_SJ(hDC, bullet_head, 124, 350);
						SunBoom_SJ(hDC, bullet_head, 842, 420);
						break;
					case 393:
						SunBoom_SJ(hDC, bullet_head, 640, 150);
						SunBoom_SJ(hDC, bullet_head, 4, 560);
						break;
					case 405:
						addBoom(head, Boom_LeftLaser, -100, 340, 1300, 390);
						addBoom(head, Boom_RightLaser, -100, 340, 1300, 390);
						addBoom(head, Boom_UpLaser, 540, -100, 590, 900);
						addBoom(head, Boom_DownLaser, 540, -100, 590, 900);
						addBoom(head, Boom_Laser2, 50, -100, 100, 900);
						addBoom(head, Boom_Laser2, 1090, -100, 1140, 900);
						addBoom(head, Boom_Laser, -100, 100, 1300, 150);
						addBoom(head, Boom_Laser, -100, 650, 1300, 700);
						break;
					case 440:
						addBoom(head, Boom_Circle, 100, 80, 500, 480);
						addBoom(head, Boom_Circle, 850, 400, 1150, 700);
						break;
					case 445:
						SunBoom_SJ(hDC, bullet_head, 124, 350);
						SunBoom_SJ(hDC, bullet_head, 842, 420);
						SunBoom_SJ(hDC, bullet_head, 640, 150);
						SunBoom_SJ(hDC, bullet_head, 4, 560);
						break;
					case 446:
						addBoom(head, Boom_Circle, 460, 260, 660, 460);
						break;
					case 448:
						addBoom(head, Boom_Circle, 360, 160, 760, 560);
						break;
					case 450:
						addBoom(head, Boom_Circle, 260, 60, 860, 660);
						break;
					case 452:
						addBoom(head, Boom_Circle, 160, -40, 960, 760);
						break;
					case 453:
						addBoom(head, Boom_Circle, 60, -140, 1060, 860);
						break;
					case 490:
						SunBoom_SJ(hDC, bullet_head, 50, 50);
						SunBoom_SJ(hDC, bullet_head, 450, 600);
						SunBoom_SJ(hDC, bullet_head, 223, 700);
						SunBoom_SJ(hDC, bullet_head, 900, 560);
						SunBoom_SJ(hDC, bullet_head, 930, 220);
						break;
					case 580:
						Doughnut(hDC, head, 520, 340, 50);
						addBoom(head, Boom_DownLaser, 50, -100, 100, 900);
						addBoom(head, Boom_DownLaser, 1120, -100, 1170, 900);
						SunBoom_SJ(hDC, bullet_head, 540, 380);
						break;
					case 581:
						addBoom(head, Boom_LeftLaser, -100, 340, 1300, 390);
						SunBoom_SJ(hDC, bullet_head, 150, 200);
						break;
					case 582:
						addBoom(head, Boom_RightLaser, -100, 340, 1300, 390);
						SunBoom_SJ(hDC, bullet_head, 250, 300);
						break;
					case 583:
						addBoom(head, Boom_UpLaser, 540, -100, 590, 900);
						SunBoom_SJ(hDC, bullet_head, 550, 300);
						break;
					case 584:
						addBoom(head, Boom_DownLaser, 540, -100, 590, 900);
						SunBoom_SJ(hDC, bullet_head, 380, 600);
						break;
					case 628:
						addBoom(head, Boom_RightLaser, -100, 100, 1300, 150);
						addBoom(head, Boom_RightLaser, -100, 200, 1300, 250);
						addBoom(head, Boom_DownLaser, 990, -100, 1040, 900);
						addBoom(head, Boom_DownLaser, 1090, -100, 1140, 900);
						addBoom(head, Boom_Rectangle, 460, 280, 660, 480);
						break;
					case 629:
						addBoom(head, Boom_LeftLaser, -100, 550, 1300, 600);
						addBoom(head, Boom_LeftLaser, -100, 650, 1300, 700);
						addBoom(head, Boom_UpLaser, 50, -100, 100, 900);
						addBoom(head, Boom_UpLaser, 150, -100, 200, 900);
						break;
					case 675:
						SunBoom_SJ(hDC, bullet_head, 580 - 350 * cos(0 * (3.1415926535897 / 180)), 350 - 350 * sin(0 * (3.1415926535897 / 180)));
						addBoom(head, Boom_Circle, 230, 0, 930, 700);
						break;
					case 676:
						SunBoom_SJ(hDC, bullet_head, 580 - 350 * cos(60 * (3.1415926535897 / 180)), 350 - 350 * sin(60 * (3.1415926535897 / 180)));
						break;
					case 677:
						SunBoom_SJ(hDC, bullet_head, 580 - 350 * cos(120 * (3.1415926535897 / 180)), 350 - 350 * sin(120 * (3.1415926535897 / 180)));
						addBoom(head, Boom_Rectangle, 230, 0, 930, 700);
						break;
					case 678:
						SunBoom_SJ(hDC, bullet_head, 580 - 350 * cos(180 * (3.1415926535897 / 180)), 350 - 350 * sin(180 * (3.1415926535897 / 180)));
						break;
					case 679:
						SunBoom_SJ(hDC, bullet_head, 580 - 350 * cos(240 * (3.1415926535897 / 180)), 350 - 350 * sin(240 * (3.1415926535897 / 180)));
						addBoom(head, Boom_Circle, 230, 0, 930, 700);
						break;
					case 680:
						SunBoom_SJ(hDC, bullet_head, 580 - 350 * cos(300 * (3.1415926535897 / 180)), 350 - 350 * sin(300 * (3.1415926535897 / 180)));
						break;
					case 681:
						SunBoom_SJ(hDC, bullet_head, 580 - 500 * cos(0 * (3.1415926535897 / 180)), 350 - 500 * sin(0 * (3.1415926535897 / 180)));
						addBoom(head, Boom_Rectangle, 230, 0, 930, 700);
						break;
					case 682:
						SunBoom_SJ(hDC, bullet_head, 580 - 500 * cos(60 * (3.1415926535897 / 180)), 350 - 500 * sin(60 * (3.1415926535897 / 180)));
						break;
					case 683:
						SunBoom_SJ(hDC, bullet_head, 580 - 500 * cos(120 * (3.1415926535897 / 180)), 350 - 500 * sin(120 * (3.1415926535897 / 180)));
						break;
					case 684:
						SunBoom_SJ(hDC, bullet_head, 580 - 500 * cos(180 * (3.1415926535897 / 180)), 350 - 500 * sin(180 * (3.1415926535897 / 180)));
						break;
					case 685:
						SunBoom_SJ(hDC, bullet_head, 580 - 500 * cos(240 * (3.1415926535897 / 180)), 350 - 500 * sin(240 * (3.1415926535897 / 180)));
						break;
					case 686:
						SunBoom_SJ(hDC, bullet_head, 580 - 500 * cos(300 * (3.1415926535897 / 180)), 350 - 500 * sin(300 * (3.1415926535897 / 180)));
						break;
					case 700:
						addBoom(head, Boom_DownLaser, 50, -100, 100, 900);
						addBoom(head, Boom_DownLaser, 1120, -100, 1170, 900);
						break;

					case 701:
						addBoom(head, Boom_DownLaser, 130, -100, 180, 900);
						addBoom(head, Boom_DownLaser, 1040, -100, 1090, 900);
						break;

					case 702:
						addBoom(head, Boom_DownLaser, 210, -100, 260, 900);
						addBoom(head, Boom_DownLaser, 960, -100, 1010, 900);
						break;

					case 703:
						addBoom(head, Boom_DownLaser, 290, -100, 340, 900);
						addBoom(head, Boom_DownLaser, 880, -100, 930, 900);
						break;

					case 755:  //승리조건
						eRound = YouWin;
						score = sj_Timer * Energybar.right;
						playSound(YOUWIN);
						menuOnOff = true;
						break;
					default:
						break;
					}
				}
				break;
			}
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_KEYDOWN:
		if (wParam == 'E' && p2TcpData.DashCnt>0)
		{
			Energybar.right -= 50;
			p1TcpData.useDash = true;
			effPlaySound(jump);
			p1TcpData.useDash = true;

			if (GetAsyncKeyState('A') < 0)
			{
				p1TcpData.playerXpos -= 100;

				if (p1TcpData.playerXpos < 12.5)
				{
					p1TcpData.playerXpos = 12.5;
				}
			}

			if (GetAsyncKeyState('W') < 0)
			{
				p1TcpData.playerYpos -= 100;

				if (p1TcpData.playerYpos < 12.5)
				{
					p1TcpData.playerYpos = 12.5;
				}
			}

			if (GetAsyncKeyState('S') < 0)
			{
				p1TcpData.playerYpos += 100;

				if (p1TcpData.playerYpos > WindowSize.bottom - 52.5)
				{
					p1TcpData.playerYpos = 52.5;
				}
			}

			if (GetAsyncKeyState('D') < 0)
			{
				p1TcpData.playerXpos += 100;

				if (p1TcpData.playerXpos > 1171.5)
				{
					p1TcpData.playerXpos = 1171.5;
				}
			}
		}
		else if (wParam == 'R')
		{
			if (Tp && p2TcpData.telCnt > 0)
			{
				p1TcpData.playerXpos = p1TcpData.teleportXpos;
				p1TcpData.playerYpos = p1TcpData.teleportYpos;

				effPlaySound(teleP);
				Tp = FALSE;
				p1TcpData.useTeleport = true;
				Energybar.right -= 50;
			}
			else if(!Tp && p2TcpData.telCnt > 0)
			{
				p1TcpData.teleportXpos = p1TcpData.playerXpos;
				p1TcpData.teleportYpos = p1TcpData.playerYpos;
				p1TcpData.useTeleport = false;
				Tp = TRUE;

			}
		}

		//if (wParam == 'O')
		//{
		//	effPlaySound(jump);

		//	if (GetAsyncKeyState('J') < 0)
		//	{
		//		Player_2.left -= 100;
		//		Player_2.right -= 100;

		//		if (Player_2.left < 0)
		//		{
		//			Player_2.left = 0;
		//			Player_2.right = 25;
		//		}
		//	}

		//	if (GetAsyncKeyState('I') < 0)
		//	{
		//		Player_2.top -= 100;
		//		Player_2.bottom -= 100;
		//		if (Player_2.top < 0)
		//		{
		//			Player_2.top = 0;
		//			Player_2.bottom = 25;
		//		}
		//	}

		//	if (GetAsyncKeyState('K') < 0)
		//	{
		//		Player_2.top += 100;
		//		Player_2.bottom += 100;

		//		if (Player_2.bottom > WindowSize.bottom - 20)
		//		{
		//			Player_2.top = WindowSize.bottom - 45;
		//			Player_2.bottom = WindowSize.bottom - 20;
		//		}
		//	}

		//	if (GetAsyncKeyState('L') < 0)
		//	{
		//		Player_2.left += 100;
		//		Player_2.right += 100;

		//		if (Player_2.right > 1184)
		//		{
		//			Player_2.left = 1159;
		//			Player_2.right = 1184;
		//		}
		//	}
		//}
		//else if (wParam == 'P')
		//{
		//	if (Tp_2)
		//	{
		//		Player_2.bottom = tmp2.bottom;
		//		Player_2.left = tmp2.left;
		//		Player_2.right = tmp2.right;
		//		Player_2.top = tmp2.top;
		//		effPlaySound(teleP);
		//		Tp_2 = FALSE;
		//	}
		//	else
		//	{

		//		tmp2.bottom = Player_2.bottom;
		//		tmp2.left = Player_2.left;
		//		tmp2.right = Player_2.right;
		//		tmp2.top = Player_2.top;
		//		Tp_2 = TRUE;

		//	}
		//}
		switch (wParam) {
		case VK_BACK:
			sj_Timer -= 10;
			break;
		case VK_F4:
			if (PauseOnOff)
				PauseOnOff = false;
			else
				PauseOnOff = true;
			SoundPause();
			break;
		case VK_F3:
			if (POWEROVERWHELMING)
				POWEROVERWHELMING = FALSE;
			else
				POWEROVERWHELMING = TRUE;
			break;
		case VK_F1:
			syc--;
			break;
		case VK_F2:
			syc++;
			break;
		}
		break;
	case WM_PAINT:
		MemDC = BeginPaint(hWnd, &ps);

		//GetClientRect(hWnd, &bufferRT);
		hDC = CreateCompatibleDC(MemDC);
		BackBit = CreateCompatibleBitmap(MemDC, WindowSize.right, WindowSize.bottom);
		oldBackBit = (HBITMAP)SelectObject(hDC, BackBit);
		PatBlt(hDC, 0, 0, WindowSize.right, WindowSize.bottom, BLACKNESS);

		DrawTelCnt(hDC, eRound, g_hInst);

		if (menuOnOff) {
			switch (DrawMenu(hDC, eRound, g_hInst))
			{
			case 1:
				KillTimer(hWnd, 0);
				KillTimer(hWnd, 1);
				SetTimer(hWnd, 0, 10, NULL);
				SetTimer(hWnd, 1, 100, NULL);

				sj_Timer = 0;
				playSound(Perion);
				menuOnOff = false;
				break;
			case 2:
				KillTimer(hWnd, 0);
				KillTimer(hWnd, 1);
				SetTimer(hWnd, 0, 10, NULL);
				SetTimer(hWnd, 1, 100, NULL);
				sj_Timer = 0;
				playSound(STAGE2);
				menuOnOff = false;
				break;
			default:
				break;
			}
		}
		else {

			Animation(hDC, g_hInst, head, bullet_head);
			//SelectObject(hDC, oldBackBit);
			DrawEnergybar(hDC, g_hInst, eRound);
			if(multiOn)
				DrawEnergybar2(hDC, g_hInst, eRound);
			GetClientRect(hWnd, &WindowSize);
		}

		

		BitBlt(MemDC, 0, 0, WindowSize.right, WindowSize.bottom, hDC, 0, 0, SRCCOPY);

		DeleteObject(BackBit);
		DeleteDC(hDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		KillTimer(hWnd, 0);
		KillTimer(hWnd, 1);
		free(head);
		DeleteObject(Teleport);
		DeleteObject(PLAYER_1);
		DeleteObject(PLAYER_2);
		DeleteObject(Circle_Boom);
		DeleteObject(Laser_Boom);
		PostQuitMessage(0);
		break;

	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


void err_quit(char* msg)
{
	LPVOID lpmsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpmsgBuf, 0, NULL);
	wchar_t* w_str;
	int len = (int)strlen(msg) + 1;
	mbstowcs(w_str, msg, len);


	MessageBox(NULL, (LPCWSTR)lpmsgBuf, w_str, MB_ICONERROR);
	LocalFree(lpmsgBuf);
	exit(1);
}

void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}


int CreateSocket()
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// tcp socket()
	TCPsock = socket(AF_INET, SOCK_STREAM, 0);
	if (TCPsock == INVALID_SOCKET) err_quit((char *)"TCPsocket()");

	// udp socket()
	UDPsock = socket(AF_INET, SOCK_DGRAM, 0);
	if (UDPsock == INVALID_SOCKET) err_quit((char *)"UDPsocket()");

	//connect()
	//serveraddr;
	ZeroMemory(&Tserveraddr, sizeof(Tserveraddr));
	Tserveraddr.sin_family = AF_INET;
	Tserveraddr.sin_addr.S_un.S_addr = inet_addr(SERVERIP);
	Tserveraddr.sin_port = htons(SERVERPORT);

	ZeroMemory(&Userveraddr, sizeof(Userveraddr));
	Userveraddr.sin_family = AF_INET;
	Userveraddr.sin_addr.S_un.S_addr = inet_addr(SERVERIP);
	Userveraddr.sin_port = htons(SERVERPORT);

	retval = connect(TCPsock, (SOCKADDR*)&Tserveraddr, sizeof(Tserveraddr));
	if (retval == SOCKET_ERROR) err_quit((char*)"connect()");

}

int sendTcpData(tcpdata playerData)
{
	int retval;
	
	//retval = connect(TCPsock, (SOCKADDR*)&Tserveraddr, sizeof(Tserveraddr));
	//if (retval == SOCKET_ERROR) err_quit((char*)"connect()");

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE];
int sendDataSize = sizeof(tcpData);

char data[256];
ZeroMemory(data, 256);

// 구조체 크기 전송
//retval = send(TCPsock, (char *)&sendDataSize, sizeof(int), 0);

playerData.teleportXpos += 1;

retval = send(TCPsock, (char*)&playerData, sizeof(playerData), 0);

p1TcpData.useDash = false;
p1TcpData.useTeleport = false;

return 0;
}

int sendUdpData(udpdata playerData)
{
	int retval;
	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE];
	int sendDataSize = sizeof(udpData);

	char data[256];
	ZeroMemory(data, 256);

	// 구조체 크기 전송
	//retval = sendto(TCPsock, (char*)&sendDataSize, sizeof(int), 0,
	//	(SOCKADDR *)&serveraddr, sizeof(serveraddr));

	retval = sendto(UDPsock, (char*)&playerData, sizeof(playerData), 0,
		(SOCKADDR*)&Userveraddr, sizeof(Userveraddr));
	return 0;
}

void recvTcpData()
{
	int retval;
	int getSize;
	char buffer[500];

	getSize = recv(TCPsock, buffer, sizeof(buffer) - 1, 0);
	buffer[getSize] = '\0';

	tcpdata* tData = (tcpdata*)buffer;

	p2TcpData = *tData;
}

void recvUdpData()
{
	int addrlen = sizeof(Userveraddr);
	int retval;
	int getSize;
	char buffer[500];

	getSize = recvfrom(UDPsock, buffer, sizeof(buffer) - 1, 0,
		(SOCKADDR*)&Userveraddr, &addrlen);
	buffer[getSize] = '\0';

	udpData* uData = (udpData*)buffer;

	p2UdpData = *uData;

	if (uData->playerID == '1')
	{
		int a = 3;
	}

}

void UpdateData()
{
	Player_1.left = p1TcpData.playerXpos - 12.5;
	Player_1.right = p1TcpData.playerXpos + 12.5;
	Player_1.top = p1TcpData.playerYpos - 12.5;
	Player_1.bottom = p1TcpData.playerYpos + 12.5;

	if (Tp)
	{
		tmp.bottom = p1TcpData.teleportYpos + 12.5;
		tmp.left = p1TcpData.teleportXpos - 12.5;
		tmp.right = p1TcpData.teleportXpos + 12.5;
		tmp.top = p1TcpData.teleportYpos - 12.5;
	}

	p1TcpData.hp = (Energybar.right) * 100 / WindowSize.right;

	//p1TcpData.sceneState = eRound;

	//if (eRound == MultiReady)
	//{
	//	if (p1TcpData.sceneState == Select)
	//	{
	//		eRound = p1TcpData.sceneState;
	//		multiSelect = true;
	//	}
	//}
	if(p1TcpData.sceneState == Round1)
	{
		int a = 3;
	}

	if (p2TcpData.sceneState == Round1)
	{
		eRound = Round1;
	}

	else if (p2TcpData.sceneState == Round2)
	{
		eRound = Round2;
	}

	Player_2.left = p2TcpData.playerXpos - 12.5;
	Player_2.right = p2TcpData.playerXpos + 12.5;
	Player_2.top = p2TcpData.playerYpos - 12.5;
	Player_2.bottom = p2TcpData.playerYpos + 12.5;

	Energybar2.right = p2TcpData.hp* WindowSize.right / 100;

	if (p1TcpData.hp < 0)
	{
		p1TcpData.sceneState = YouDie;
		eRound = YouDie;
	}
	else if (p2TcpData.hp < 0)
	{
		p1TcpData.sceneState = YouWin;
		eRound = YouWin;
		score = sj_Timer * Energybar.right;
	}
}