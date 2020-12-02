#include "stdafx.h"
#include "CTCPsocket.h"
#include "Utility.h"
#include "CPlayer.h"
#include "CUDPsocket.h"

int g_clientCnt = 0;
// ���� �Լ� ���� ��� �� ����


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
	//�̺�Ʈ ����
	//player1
	player1.m_state = EROUND::Select;
	player2.m_state = EROUND::Select;
	g_hTCPSendEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_hTCPSendEvent == NULL) return 1;

	g_hTCPRecvEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_hTCPRecvEvent == NULL) return 1;



	g_hUDPEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (g_hUDPEvent == NULL)
		return 1;



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
			//break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// ������ ����
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)&tcpSocket, 0, NULL);

		//hThread = CreateThread(NULL, 0, UdpClient, (LPVOID)&tcpSocket, 0, NULL);
		/*if (hThread == NULL)
		{
			closesocket(client_sock);
		}
		else
		{
			CloseHandle(hThread);
		}*/

		//TCPRecvEvent�� �����⸦ ��ٸ���.


		//UDPEvent�Ϸ�


	}

	// ���� ����
	WSACleanup();
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
	while (1) {




		//cout << "-------------------------\n";
		//cout << "              " << index << ' ' << g_clientCnt << '\n';
		//cout << "-------------------------\n";


		//retval = WaitForSingleObject(g_hUDPEvent, INFINITE);


		//cout << "tcprecv������ �ޱ� �Ѵ�." << endl;
		tcpSocket->TcpRecvData(index, client_sock);
		//cout << "tcprecv������ �ޱ� �Ϸ�." << endl;

		//cout << "tcprecv������ ������ �Ѵ�." << endl;
		tcpSocket->TcpSendData(index, client_sock);
		//cout << "tcprecv������ ������ �Ϸ�." << endl;




		//cout << "udp������ �ޱ�" << endl;
		//udpSocket.UDPRecvData(index);
		//cout << "udp������ �ޱ� �Ϸ�" << endl;
		//

		//cout << "udp������\n	";
		//udpSocket.UDPSendData(index);
		//cout << "udp������Ϸ�\n	";

		//retval = WaitForSingleObject(g_hUDPEvent, INFINITE);
		
		//ResetEvent(g_hUDPEvent);
		//SetEvent(g_hTCPRecvEvent);

		/*ResetEvent(g_hTCPRecvEvent);
		SetEvent(g_hTCPSendEvent);*/
	}

	// closesocket()
	closesocket(tcpSocket->GetClientSock());
	printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(tcpSocket->GetClientaddr().sin_addr), ntohs(tcpSocket->GetClientaddr().sin_port));

	return 0;
}