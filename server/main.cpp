#include "stdafx.h"
#include "CTCPsocket.h"
#include "Utility.h"
#include "CPlayer.h"
#include "CUDPsocket.h"


CRITICAL_SECTION g_cs;
int g_clientCnt = 0;
// ���� �Լ� ���� ��� �� ����

//����Ƚ��
int g_TelCnt=5;
int g_DashCnt=5	;

char g_Id = ' '; //���������� ���� id;
CPlayer player1;
CPlayer player2;

//�̺�Ʈ �ڵ�
HANDLE g_hTCPSendEvent;
HANDLE g_hTCPRecvEvent;

HANDLE g_hUDPEvent;



CUDPsocket udpSocket;
DWORD WINAPI ProcessClient(LPVOID arg);
DWORD WINAPI UdpClient(LPVOID arg);
int main(int argc, char* argv[])
{
	InitializeCriticalSection(&g_cs);
	//�̺�Ʈ ����
	//player1


	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()  tcp ����, bind, listen ���� ��

	CTCPsocket tcpSocket = CTCPsocket(&player1, &player2);
	udpSocket = CUDPsocket(&player1, &player2);
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
			break;
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

		

	}

	// ���� ����
	WSACleanup();
	DeleteCriticalSection(&g_cs);
	return 0;
}


// Ŭ���̾�Ʈ�� ������ ��� tcp
DWORD WINAPI UdpClient(LPVOID arg)
{
	int retval = 0;
	while (1) {
		retval = WaitForSingleObject(g_hTCPSendEvent, INFINITE);
		cout << "udp������ �ޱ�,����" << endl;
		udpSocket.UDPRecvData();
		udpSocket.UDPSendData();
		cout << "udp�Ϸ�\n";
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
	//cout << g_clientCnt << "���� �Ϸ�" << endl;
	int index = g_clientCnt;
	CTCPsocket* tcpSocket = (CTCPsocket*)arg;
	SOCKET client_sock = tcpSocket->GetClientSock();
	SOCKADDR_IN clientaddr = tcpSocket->GetClientaddr();
	int addrlen = sizeof(clientaddr);
	int retval = 0;
	//Ŭ���̾�Ʈ ���� ���
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	tcpSocket->m_pPlayer1->Init();
	tcpSocket->m_pPlayer2->Init();
	while (1) {




		//cout << "-------------------------\n";
		//cout << "              " << index << ' ' << g_clientCnt << '\n';
		//cout << "-------------------------\n";


		//retval = WaitForSingleObject(g_hUDPEvent, INFINITE);


		//cout << "tcprecv������ �ޱ� �Ѵ�." << endl;
		if (tcpSocket->TcpRecvData(index, client_sock) == 0)
			break;
		//cout << "tcprecv������ �ޱ� �Ϸ�." << endl;

		//cout << "tcprecv������ ������ �Ѵ�." << endl;

		if (tcpSocket->TcpSendData(index, client_sock) == 0)
			break;
		//cout << "tcprecv������ ������ �Ϸ�." << endl;

		cout <<g_clientCnt<<" " << g_DashCnt <<" " << g_TelCnt << '\r';
		


	

	}
	//
	// closesocket()

	g_clientCnt = 0;
	closesocket(tcpSocket->GetClientSock());
	printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(tcpSocket->GetClientaddr().sin_addr), ntohs(tcpSocket->GetClientaddr().sin_port));

	return 0;
}