#include "stdafx.h"
#include "CTCPsocket.h"
#include "Utility.h"
#include "CPlayer.h"
#include "CUDPsocket.h"

int g_clientCnt = 0;
// ���� �Լ� ���� ��� �� ����



CPlayer player1;
CPlayer player2;

//�̺�Ʈ �ڵ�
HANDLE g_hTCPSendEvent;
HANDLE g_hTCPRecvEvent;

HANDLE g_hTCPSendEvent2;
HANDLE g_hTCPRecvEvent2;
HANDLE g_hUDPEvent;
HANDLE g_hUDPEvent2;

int main(int argc, char* argv[])
{
	//�̺�Ʈ ����
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

		
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()  tcp ����, bind, listen ���� ��

	CTCPsocket tcpSocket = CTCPsocket(&player1, &player2);
	CUDPsocket udpSocket = CUDPsocket(&player1, &player2);





	// ������ ��ſ� ����� ����
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

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// ������ ����
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)&tcpSocket, 0, NULL);
		if (hThread == NULL)
		{
			closesocket(client_sock);
		}
		else
		{
			CloseHandle(hThread);
		}

		//TCPRecvEvent�� �����⸦ ��ٸ���.
		retval = WaitForSingleObject(g_hTCPSendEvent, INFINITE);
		if (g_clientCnt == 2)
			retval = WaitForSingleObject(g_hTCPSendEvent2, INFINITE);


		cout << "udp������ �ޱ�" << endl;
		//udpSocket.UDPRecvData();
		//udpSocket.UDPSendData();
		cout << "udp�Ϸ�\n	" << endl;

		//UDPEvent�Ϸ�
		ResetEvent(g_hTCPSendEvent);
		if (g_clientCnt == 2)
			ResetEvent(g_hTCPSendEvent2);


		SetEvent(g_hUDPEvent);
		if (g_clientCnt == 2)
			SetEvent(g_hUDPEvent2);

	}

	// ���� ����
	WSACleanup();
	return 0;
}