#include "stdafx.h"
#include "CTCPsocket.h"
#include "Utility.h"
#include "CPlayer.h"
#include "CUDPsocket.h"


CRITICAL_SECTION g_cs;
int g_clientCnt = 0;
// 소켓 함수 오류 출력 후 종료

//텔포횟수
int g_TelCnt=5;
int g_DashCnt=5	;

char g_Id = ' '; //마지막으로 받은 id;
CPlayer player1;
CPlayer player2;

//이벤트 핸들
HANDLE g_hTCPSendEvent;
HANDLE g_hTCPRecvEvent;

HANDLE g_hUDPEvent;



CUDPsocket udpSocket;
DWORD WINAPI ProcessClient(LPVOID arg);
DWORD WINAPI UdpClient(LPVOID arg);
int main(int argc, char* argv[])
{
	InitializeCriticalSection(&g_cs);
	//이벤트 생성
	//player1


	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()  tcp 소켓, bind, listen 까지 함

	CTCPsocket tcpSocket = CTCPsocket(&player1, &player2);
	udpSocket = CUDPsocket(&player1, &player2);
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
			break;
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

		

	}

	// 윈속 종료
	WSACleanup();
	DeleteCriticalSection(&g_cs);
	return 0;
}


// 클라이언트와 데이터 통신 tcp
DWORD WINAPI UdpClient(LPVOID arg)
{
	int retval = 0;
	while (1) {
		retval = WaitForSingleObject(g_hTCPSendEvent, INFINITE);
		cout << "udp데이터 받기,전송" << endl;
		udpSocket.UDPRecvData();
		udpSocket.UDPSendData();
		cout << "udp완료\n";
		ResetEvent(g_hTCPSendEvent);
		SetEvent(g_hUDPEvent);
	}
	return 0;
}
DWORD WINAPI ProcessClient(LPVOID arg)
{

	g_DashCnt = 5;
	g_TelCnt = 5;
	g_clientCnt++;
	//cout << g_clientCnt << "접속 완료" << endl;
	int index = g_clientCnt;
	CTCPsocket* tcpSocket = (CTCPsocket*)arg;
	SOCKET client_sock = tcpSocket->GetClientSock();
	SOCKADDR_IN clientaddr = tcpSocket->GetClientaddr();
	int addrlen = sizeof(clientaddr);
	int retval = 0;
	//클라이언트 정보 얻기
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	tcpSocket->m_pPlayer1->Init();
	tcpSocket->m_pPlayer2->Init();
	while (1) {




		//cout << "-------------------------\n";
		//cout << "              " << index << ' ' << g_clientCnt << '\n';
		//cout << "-------------------------\n";


		//retval = WaitForSingleObject(g_hUDPEvent, INFINITE);


		//cout << "tcprecv데이터 받기 한다." << endl;
		if (tcpSocket->TcpRecvData(index, client_sock) == 0)
			break;
		//cout << "tcprecv데이터 받기 완료." << endl;

		//cout << "tcprecv데이터 보내기 한다." << endl;

		if (tcpSocket->TcpSendData(index, client_sock) == 0)
			break;
		//cout << "tcprecv데이터 보내기 완료." << endl;

		cout <<g_clientCnt<<" " << g_DashCnt <<" " << g_TelCnt << '\r';
		


	

	}
	//
	// closesocket()

	g_clientCnt = 0;
	closesocket(tcpSocket->GetClientSock());
	printf("\n[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(tcpSocket->GetClientaddr().sin_addr), ntohs(tcpSocket->GetClientaddr().sin_port));

	return 0;
}