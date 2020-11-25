#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>
#include "CPlayer.h"

//#define SERVERIP "127.0.0.1"
#define SERVERIP "192.168.219.100"
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




//���� �� �� ���� ��� �� ����
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


//�����Լ� �������
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


//����� ���� ������ ���� �Լ�
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

	//���� �ʱ�ȭ
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

	//������ ��ſ� ����� ����
	while (1) {
		char buffer[BUFSIZ];
		TCPDATA* tcpData;
		myPOINT telPos;// 
		int id = 0;

		
		retval = recv(sock, buffer, BUFSIZ-1, 0);
		cout << "�����͸� �޽��ϴ�.\n";
		cout << "main x��ǥ" << mainPlayer.GetTelePos().x << '\n';
		cout << "other x��ǥ" << otherPlayer.GetTelePos().x << '\n';
	
		buffer[retval] = '\0';
		tcpData = (TCPDATA*)buffer;
		otherPlayer.SetUseTeleport(tcpData->useTeleport);
		telPos.x = tcpData->teleportXpos;
		telPos.y = tcpData->teleportYpos;
		otherPlayer.SetTelePos(telPos);
		myPOINT t;
		t.y = 0;
		cout << "������ main�� x��ǥ �Է�\n";
		cin >> t.x;
		

		id = tcpData->playerID;
		
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

	//���� ����
	WSACleanup();
	return 0;
}