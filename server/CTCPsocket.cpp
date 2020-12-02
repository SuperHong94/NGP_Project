#include "stdafx.h"
#include "Utility.h"
#include "CTCPsocket.h"
#include "CPlayer.h"
#include "CUDPsocket.h"
CTCPsocket::CTCPsocket()
{
	//tcp 대기 소켓 생성
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
	//tcp 대기 소켓 생성
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
	//Player 1에게는 Player 2정보를 보낸다.
	tcpdata tcpData;
	//ZeroMemory(&tcpData, 0);
	myPOINT pos;



	if (m_pPlayer1->m_state == Round1 && m_pPlayer2->m_state == MultiReady)
		m_pPlayer2->m_state = Round1;
	else if (m_pPlayer2->m_state == Round1 && m_pPlayer1->m_state == MultiReady)
		m_pPlayer1->m_state = Round1;
	else if (m_pPlayer1->m_state == Round2 && m_pPlayer2->m_state == MultiReady)
		m_pPlayer2->m_state = Round2;
	else if (m_pPlayer2->m_state == Round2 && m_pPlayer1->m_state == MultiReady)
		m_pPlayer1->m_state = Round2;



	switch (index)
	{
	case 1:
		tcpData.type = 't';
		tcpData.playerID = '1';
		tcpData.useTeleport = m_pPlayer2->GetUseTeleport();
		pos = m_pPlayer2->GetTelePos();
		tcpData.teleportXpos = pos.x;
		tcpData.teleportYpos = pos.y;
		tcpData.useDash = m_pPlayer2->GetuseDash();
		tcpData.hp = m_pPlayer2->GetHp();
		pos = m_pPlayer2->Getpos();
		tcpData.playerXpos = pos.x;
		tcpData.playerYpos = pos.y;
		tcpData.sceneState = m_pPlayer1->m_state;
		
		break;
	case 2:
		tcpData.type = 't';
		tcpData.playerID = '2';
		tcpData.useDash = false;
		tcpData.useTeleport = false;
		tcpData.useTeleport = m_pPlayer1->GetUseTeleport();
		pos = m_pPlayer1->GetTelePos();
		tcpData.teleportXpos = pos.x;
		tcpData.teleportYpos = pos.y;
		tcpData.useDash = m_pPlayer1->GetuseDash();
		tcpData.hp = m_pPlayer1->GetHp();
		pos = m_pPlayer1->Getpos();
		tcpData.playerXpos = pos.x;
		tcpData.playerYpos = pos.y;
		tcpData.sceneState = m_pPlayer2->m_state;
		break;

	default:
		break;
	}
	tcpData.useDash = false;
	tcpData.useTeleport = false;
	tcpData.telCnt = g_TelCnt;
	tcpData.dashCnt = g_DashCnt;

	int size = sizeof(tcpdata);
	int retval = send(sock, (char*)&tcpData, size, 0);

	//std::cout << "보내야 되는 데이터량: " << size << " 보낸 데이터량" << retval << ' ' << tcpData.playerID << "의 telX:" << tcpData.teleportXpos << "를 보냅니다.\n";
	if (retval == SOCKET_ERROR) {
		err_display("sendtcp()");
		return;
	}

	//cout << "player1: " << m_pPlayer1->m_state << "player2: " << m_pPlayer2->m_state << '\r';

}

void CTCPsocket::TcpRecvData(int index, SOCKET sock)
{

	tcpdata* tcpData;
	char buffer[512];
	myPOINT telPos;


	//cout << "데이터를 받습니다.\n";

	int retval = recv(sock, buffer, 512 - 1, 0);
	//index가 1이면 플레이어1이 보낸정보이다.
	buffer[retval] = '\0';

	tcpData = (tcpdata*)buffer;

	//std::cout << "받은데이터량:  " << retval << ' ' << tcpData->playerID << "에서 받은 데이터: " << tcpData->teleportXpos << endl;
	switch (index)
	{
	case 1:
		m_pPlayer1->SetUseTeleport(tcpData->useTeleport);
		telPos.x = tcpData->teleportXpos;
		telPos.y = tcpData->teleportYpos;
		m_pPlayer1->SetTelePos(telPos);
		telPos.x = tcpData->playerXpos;
		telPos.y = tcpData->playerYpos;
		m_pPlayer1->SetPos(telPos);
		m_pPlayer1->SetHP(tcpData->hp);
		m_pPlayer1->m_state = tcpData->sceneState;
		break;
	case 2:
		m_pPlayer2->SetUseTeleport(tcpData->useTeleport);
		telPos.x = tcpData->teleportXpos;
		telPos.y = tcpData->teleportYpos;
		m_pPlayer2->SetTelePos(telPos);
		telPos.x = tcpData->playerXpos;
		telPos.y = tcpData->playerYpos;
		m_pPlayer2->SetPos(telPos);
		m_pPlayer2->SetHP(tcpData->hp);
		m_pPlayer2->m_state = tcpData->sceneState;
		break;
	default:
		break;
	}
	//g_TelCnt = tcpData->telCnt;
	//g_DashCnt = tcpData->dashCnt;
	EnterCriticalSection(&g_cs);
	if (tcpData->useDash)
	{
		g_DashCnt -= 1;
	}
	if (tcpData->useTeleport)
	{
		g_TelCnt -= 1;
	}
	LeaveCriticalSection(&g_cs);
	//cout << "player1: " << m_pPlayer1->m_state  << "player2: " << m_pPlayer2->m_state << '\r';


}



