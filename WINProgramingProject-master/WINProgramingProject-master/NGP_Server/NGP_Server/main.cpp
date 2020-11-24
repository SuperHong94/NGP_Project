#include "stdafx.h"
#include "CTCPsocket.h"
#include "Utility.h"
#include "CPlayer.h"


int g_clientCnt = 0;
// 소켓 함수 오류 출력 후 종료



CPlayer player1;
CPlayer player2;




int main(int argc, char* argv[])
{
	int retval;
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()  tcp 소켓, bind, listen 까지 함
	
	CTCPsocket tcpSocket=CTCPsocket(&player1,&player2);


	SOCKET udp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (udp_sock == INVALID_SOCKET) err_quit("socket()");



	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;


	while (1) {
		// accept()
		addrlen = sizeof(tcpSocket.GetClientaddr());
		clientaddr = tcpSocket.GetClientaddr();
		client_sock = tcpSocket.TCPAccept();
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 스레드 생성
		hThread = CreateThread(NULL, 0, ProcessClient,
			(LPVOID)&tcpSocket, 0, NULL);
		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }
	}

	// 윈속 종료
	WSACleanup();
	return 0;
}