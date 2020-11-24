#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>
#include "CPlayer.h"

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 1200

using namespace std;
#pragma pack(1)
struct TCPDATA
{
	char type;
	char playerID;
	bool useTeleport;
	int teleportXpos;
	int teleportYpos;
	bool useDash;
	int hp;
};
#pragma pack()

CPlayer mainPlayer;
CPlayer otherPlayer;




//소켓 함 수 오류 출력 후 종료
void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}


//소켓함수 오류출력
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}


//사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

int main(int argc, char* argv[])
{
	int retval;

	//윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	//socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");


	//connet()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)err_quit("connect()");

	//데이터 통신에 사용할 변수
	while (1) {
		char buffer[BUFSIZ];
		TCPDATA* tcpData;
		myPOINT telPos;// telPoㄴ
		int id = 0;

		cout << "데이터를 받습니다.\n";
		retval = recv(sock, buffer, BUFSIZ-1, 0);

		buffer[retval] = '\0';
		tcpData = (TCPDATA*)buffer;
		otherPlayer.SetUseTeleport(tcpData->useTeleport);
		telPos.x = tcpData->teleportXpos;
		telPos.y = tcpData->teleportYpos;
		otherPlayer.SetTelePos(telPos);
		cout << "main x좌표" << mainPlayer.GetTelePos().x << '\n';
		cout << "other x좌표" << otherPlayer.GetTelePos().x << '\n';

		id = tcpData->playerID;
		myPOINT t;
		t.y = 0;
		cout << "갱신할 main의 x좌표 입력\n";
		cin >> t.x;
		if (t.x == 0)
			break;
		mainPlayer.SetTelePos(t);
		tcpData->useTeleport = mainPlayer.GetUseTeleport();
		tcpData->teleportXpos = mainPlayer.GetTelePos().x;
		tcpData->teleportYpos = mainPlayer.GetTelePos().y;
		tcpData->useDash = mainPlayer.GetuseDash();

		retval = send(sock, (char*)tcpData, sizeof(TCPDATA), 0);

	}
	//closesoket()
	closesocket(sock);

	//윈속 종료
	WSACleanup();
	return 0;
}