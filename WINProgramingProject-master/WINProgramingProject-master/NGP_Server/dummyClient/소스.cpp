#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>
#include "CPlayer.h"

#define SERVERIP "127.0.0.1"
//#define SERVERIP "192.168.219.100"
#define SERVERPORT 9000
#define BUFSIZE 1200

using namespace std;



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

	TCPDATA tData;
	tData.hp = 100;
	tData.playerID = '1';
	tData.teleportXpos = 0;
	tData.teleportYpos = 0;
	tData.type = 't';
	udpData* uData=NULL;
	uData = new udpData;
	//connet()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)err_quit("connect()");
	char id = ' ';
	//데이터 통신에 사용할 변수


	SOCKADDR_IN peeraddr;
	int addrlen;
	int len;
	DWORD WINAPI UdpClient(LPVOID arg);
	DWORD WINAPI ProcessClient(LPVOID arg);
	while (1) {
		char buffer[BUFSIZ];
		TCPDATA* tcpData;
		myPOINT telPos;// 

		cout << "tcp받기시작\n";
		retval = recv(sock, buffer, BUFSIZ - 1, 0);
		cout << "tcp받기 완료\n";

		buffer[retval] = '\0';
		tcpData = (TCPDATA*)buffer;
		id = tcpData->playerID;
		otherPlayer.SetUseTeleport(tcpData->useTeleport);
		telPos.x = tcpData->teleportXpos;
		telPos.y = tcpData->teleportYpos;
		otherPlayer.SetTelePos(telPos);

		tcpData->playerID = id;
		tcpData->useTeleport = mainPlayer.GetUseTeleport();
		tcpData->teleportXpos = mainPlayer.GetTelePos().x;
		tcpData->teleportYpos = mainPlayer.GetTelePos().y;
		tcpData->useDash = mainPlayer.GetuseDash();
		cout << "tcp보내기시작\n";
		retval = send(sock, (char*)tcpData, sizeof(TCPDATA), 0);
		cout << "tcp보내기 완료\n";


		uData->playerID = id;
		telPos = mainPlayer.Getpos();
		uData->playerXpos = telPos.x;
		uData->playerYpos = telPos.y;
		uData->sceneState = mainPlayer.m_state;
		cout << "udp보내기시작\n";
		retval = sendto(sock, (char*)uData, sizeof(udpdata), 0,
			(SOCKADDR*)&serveraddr, sizeof(serveraddr));
		cout << "udp보내기완료\n";
		int size = sizeof(serveraddr);

		ZeroMemory(buffer, 512);
		addrlen = sizeof(peeraddr);
		cout << "udp받기시작\n";
		retval = recvfrom(sock, buffer, 512 - 1, 0,
			(SOCKADDR*)&peeraddr, &addrlen);
		cout << "udp받기 완료\n";
		buffer[retval] = '\0';
		uData = (udpData*)buffer;
		if (uData->playerID == id) {
			telPos.x = uData->playerXpos;
			telPos.y = uData->playerYpos;
			otherPlayer.SetPos(telPos);
			otherPlayer.m_state=uData->sceneState;
		}





		
		cout << "-------플레이어 정보------\n";
		cout << "id " << id << ' ' << mainPlayer.Getpos().x << endl;
		cout << "\n다른 플레이어 정보\n"<<otherPlayer.Getpos().x<<endl;
		cout << "바꿀 x좌표 입력: ";
		cin >> telPos.x;
		mainPlayer.SetPos(telPos);
	}
	//closesoket()
	closesocket(sock);

	//윈속 종료
	WSACleanup();
	return 0;
}

DWORD WINAPI ProcessClient(LPVOID arg);