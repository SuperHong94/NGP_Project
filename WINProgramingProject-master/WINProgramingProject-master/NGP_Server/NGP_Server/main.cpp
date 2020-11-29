#include "stdafx.h"
#include "CTCPsocket.h"
#include "Utility.h"
#include "CPlayer.h"
#include "CUDPsocket.h"

int g_clientCnt = 0;
// ���� �Լ� ���� ��� �� ����



CPlayer player1;
CPlayer player2;




int main(int argc, char* argv[])
{
	int retval=0;
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()  tcp ����, bind, listen ���� ��
	
	CTCPsocket tcpSocket=CTCPsocket(&player1,&player2);
	CUDPsocket udpSocket = CUDPsocket(&player1, &player2);

	SOCKET udp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (udp_sock == INVALID_SOCKET) err_quit("socket()");



	// ������ ��ſ� ����� ����
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

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// ������ ����
		hThread = CreateThread(NULL, 0, ProcessClient,
			(LPVOID)&tcpSocket, 0, NULL);
		if (hThread == NULL) { closesocket(client_sock); }
		else { CloseHandle(hThread); }
		udpSocket.UDPRecvData();
		udpSocket.UDPSendData();
		
	}

	// ���� ����
	WSACleanup();
	return 0;
}