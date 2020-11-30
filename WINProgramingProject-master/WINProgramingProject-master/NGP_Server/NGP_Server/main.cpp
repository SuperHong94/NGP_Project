#include "stdafx.h"
#include "CTCPsocket.h"
#include "Utility.h"
#include "CPlayer.h"
#include "CUDPsocket.h"

int g_clientCnt = 0;
// 소켓 함수 오류 출력 후 종료



CPlayer player1;
CPlayer player2;

//이벤트 핸들
HANDLE g_hTCPSendEvent;
HANDLE g_hTCPRecvEvent;

HANDLE g_hTCPSendEvent2;
HANDLE g_hTCPRecvEvent2;
HANDLE g_hUDPEvent;
HANDLE g_hUDPEvent2;

int main(int argc, char* argv[])
{
	//이벤트 생성
	//player1
	g_hTCPSendEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_hTCPSendEvent == NULL) return 1;

	g_hTCPRecvEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (g_hTCPRecvEvent == NULL) return 1;

	//player2
	g_hTCPSendEvent2 = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_hTCPSendEvent2 == NULL) return 1;

	g_hTCPRecvEvent2 = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (g_hTCPRecvEvent2 == NULL) return 1;


	g_hUDPEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_hUDPEvent == NULL)
		return 1;

	g_hUDPEvent2 = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_hUDPEvent2 == NULL)
		return 1;

		
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()  tcp 소켓, bind, listen 까지 함

	CTCPsocket tcpSocket = CTCPsocket(&player1, &player2);
	CUDPsocket udpSocket = CUDPsocket(&player1, &player2);





	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;


	int retval = 0;
	// accept()
	while (1) {
		addrlen = sizeof(tcpSocket.GetClientaddr());
		clientaddr = tcpSocket.GetClientaddr();
		client_sock = tcpSocket.TCPAccept();
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			//break;
		}

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 스레드 생성
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)&tcpSocket, 0, NULL);
		if (hThread == NULL)
		{
			closesocket(client_sock);
		}
		else
		{
			CloseHandle(hThread);
		}

		//TCPRecvEvent가 끝나기를 기다린다.
		retval = WaitForSingleObject(g_hTCPSendEvent, INFINITE);
		if (g_clientCnt == 2)
			retval = WaitForSingleObject(g_hTCPSendEvent2, INFINITE);


		cout << "udp데이터 받기" << endl;
		//udpSocket.UDPRecvData();
		//udpSocket.UDPSendData();
		cout << "udp완료\n	" << endl;

		//UDPEvent완료
		ResetEvent(g_hTCPSendEvent);
		if (g_clientCnt == 2)
			ResetEvent(g_hTCPSendEvent2);


		SetEvent(g_hUDPEvent);
		if (g_clientCnt == 2)
			SetEvent(g_hUDPEvent2);

	}

	// 윈속 종료
	WSACleanup();
	return 0;
}