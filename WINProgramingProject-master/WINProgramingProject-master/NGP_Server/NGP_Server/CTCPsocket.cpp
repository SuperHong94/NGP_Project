#include "stdafx.h"
#include "Utility.h"
#include "CTCPsocket.h"
#include "CPlayer.h"
CTCPsocket::CTCPsocket()
{
	//tcp ��� ���� ����
	m_listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listen_sock == INVALID_SOCKET) err_quit("socket()");

	//bind()
	ZeroMemory(&m_serveraddr, sizeof(m_serveraddr));
	m_serveraddr.sin_family = AF_INET;
	m_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_serveraddr.sin_port = htons(SERVERPORT);
	int retval = bind(m_listen_sock, (SOCKADDR*)&m_serveraddr, sizeof(m_serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");


	//listen()
	retval = listen(m_listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");


	m_pPlayer1 = NULL;
	m_pPlayer2 = NULL;

}

CTCPsocket::CTCPsocket(CPlayer* p1, CPlayer* p2)
{
	//tcp ��� ���� ����
	m_listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listen_sock == INVALID_SOCKET) err_quit("socket()");

	//bind()
	ZeroMemory(&m_serveraddr, sizeof(m_serveraddr));
	m_serveraddr.sin_family = AF_INET;
	m_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_serveraddr.sin_port = htons(SERVERPORT);
	int retval = bind(m_listen_sock, (SOCKADDR*)&m_serveraddr, sizeof(m_serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");


	//listen()
	retval = listen(m_listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");
	m_pPlayer1 = p1;
	m_pPlayer2 = p2;
}

CTCPsocket::~CTCPsocket()
{
}

SOCKET CTCPsocket::GetListenSock()
{
	return m_listen_sock;
}

SOCKET CTCPsocket::GetClientSock()
{
	return m_client_sock;
}

SOCKADDR_IN CTCPsocket::GetServeraddr()
{
	return m_serveraddr;
}

SOCKADDR_IN CTCPsocket::GetClientaddr()
{
	return m_clientaddr;
}

SOCKET CTCPsocket::TCPAccept()
{
	int addrlen = sizeof(m_clientaddr);
	m_client_sock = accept(m_listen_sock, (SOCKADDR*)&m_clientaddr, &addrlen);
	return m_client_sock;
}

void CTCPsocket::TcpSendData(int index, SOCKET sock)
{
	//Player 1���Դ� Player 2������ ������.
	tcpdata tcpData;
	//ZeroMemory(&tcpData, 0);
	switch (index)
	{
	case 1:
		tcpData.type = 't';
		tcpData.playerID = '2';
		tcpData.useTeleport = m_pPlayer2->GetUseTeleport();
		tcpData.teleportXpos = m_pPlayer2->GetTelePos().x;
		tcpData.teleportYpos = m_pPlayer2->GetTelePos().y;
		tcpData.useDash = m_pPlayer2->GetuseDash();
		tcpData.hp = m_pPlayer2->GetHp();
		break;
	case 2:
		tcpData.type = 't';
		tcpData.playerID = '1';
		tcpData.useTeleport = m_pPlayer1->GetUseTeleport();
		tcpData.teleportXpos = m_pPlayer1->GetTelePos().x;
		tcpData.teleportYpos = m_pPlayer1->GetTelePos().y;
		tcpData.useDash = m_pPlayer1->GetuseDash();
		tcpData.hp = m_pPlayer1->GetHp();
		break;

	default:
		break;
	}

	int size = sizeof(tcpdata);
	int retval = send(sock, (char*)&tcpData, size, 0);

	std::cout << "������ �Ǵ� �����ͷ�: " << size << " ���� �����ͷ�" << retval << ' ' << tcpData.playerID << "�� telX:" << tcpData.teleportXpos << "�� �����ϴ�.\n";
	if (retval == SOCKET_ERROR) {
		err_display("sendtcp()");
		return;
	}



}

void CTCPsocket::TcpRecvData(int index, SOCKET sock)
{

	tcpdata* tcpData;
	char buffer[512];
	myPOINT telPos;


	cout << "�����͸� �޽��ϴ�.\n";

	int retval = recv(sock, buffer, 512 - 1, 0);
	//index�� 1�̸� �÷��̾�1�� ���������̴�.
	buffer[retval] = '\0';

	tcpData = (tcpdata*)buffer;

	std::cout << "���������ͷ�:  " << retval << ' ' << tcpData->playerID << "���� ���� ������: " << tcpData->teleportXpos << endl;
	switch (index)
	{
	case 1:
		m_pPlayer1->SetUseTeleport(tcpData->useTeleport);
		telPos.x = tcpData->teleportXpos;
		telPos.y = tcpData->teleportYpos;
		m_pPlayer1->SetTelePos(telPos);
		m_pPlayer1->SetHP(tcpData->hp);
		break;
	case 2:
		m_pPlayer2->SetUseTeleport(tcpData->useTeleport);
		telPos.x = tcpData->teleportXpos;
		telPos.y = tcpData->teleportYpos;
		m_pPlayer2->SetTelePos(telPos);
		m_pPlayer2->SetHP(tcpData->hp);
		break;
	default:
		break;
	}
}



// Ŭ���̾�Ʈ�� ������ ��� tcp
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
		cout << "recvEvent��ٸ���" << endl;
		if (index == 1)
			retval = WaitForSingleObject(g_hTCPRecvEvent, INFINITE);
		else if (index == 2) {
			retval = WaitForSingleObject(g_hTCPRecvEvent2, INFINITE);
		}
		cout << "tcprecv������ ������ �Ѵ�." << endl;
		tcpSocket->TcpSendData(index, client_sock);
		cout << "tcprecv������ ������ �Ϸ�." << endl;
		//tcp�̺�Ʈ����ȭ �̺�Ʈ�� udp�� �ѱ�� �κ�
		//tcpSend�Ϸ� �˸�
		if (index == 1) {
			ResetEvent(g_hTCPRecvEvent);
			SetEvent(g_hTCPSendEvent);
		}
		else if (index == 2)
		{
			ResetEvent(g_hTCPRecvEvent2);
			SetEvent(g_hTCPSendEvent2);
		}




		//UDP�����Ͱ� �����⸦ ��ٸ���.
		if (index == 1)
			retval = WaitForSingleObject(g_hUDPEvent, INFINITE);
		else if (index == 2)
			retval = WaitForSingleObject(g_hUDPEvent2, INFINITE);
		cout << "tcprecv������ �ޱ� �Ѵ�." << endl;
		tcpSocket->TcpRecvData(index, client_sock);
		cout << "tcprecv������ �ޱ� �Ϸ�." << endl;
		//TCP���ú� �ޱ� �Ϸ�
		if (index == 1) {
			ResetEvent(g_hUDPEvent);
			SetEvent(g_hTCPRecvEvent);
		}
		else if (index == 2) {
			ResetEvent(g_hUDPEvent2);
			SetEvent(g_hTCPRecvEvent2);
		}

	}

	// closesocket()
	closesocket(tcpSocket->GetClientSock());
	printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(tcpSocket->GetClientaddr().sin_addr), ntohs(tcpSocket->GetClientaddr().sin_port));

	return 0;
}