#include "stdafx.h"
#include "Utility.h"
#include "CTCPsocket.h"
#include "CPlayer.h"
CTCPsocket::CTCPsocket()
{
	//tcp 대기 소켓 생성
	m_listen_sock= socket(AF_INET, SOCK_STREAM, 0);
	if (m_listen_sock == INVALID_SOCKET) err_quit("socket()");

	//bind()
	ZeroMemory(&m_serveraddr, sizeof(m_serveraddr));
	m_serveraddr.sin_family = AF_INET;
	m_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_serveraddr.sin_port = htons(SERVERPORT);
	int retval=bind(m_listen_sock, (SOCKADDR*)&m_serveraddr, sizeof(m_serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");


	//listen()
	retval = listen(m_listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");


	m_pPlayer1 =NULL;
	m_pPlayer2 =NULL;

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

void CTCPsocket::TcpSendData(int index,SOCKET sock)
{
	//Player 1에게는 Player 2정보를 보낸다.
	tcpdata tcpData;
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
	int retval = send(sock, (char*)&tcpData, sizeof(tcpdata), 0);
	if (retval == SOCKET_ERROR) {
		err_display("sendtcp()");
		return ;
	}


	
}

void CTCPsocket::TcpRecvData(int index, SOCKET socket)
{
}



// 클라이언트와 데이터 통신 tcp
DWORD WINAPI ProcessClient(CTCPsocket* arg)
{
	g_clientCnt++;
	int index = g_clientCnt;
	CTCPsocket* tcpSocket = arg;
	SOCKADDR_IN clientaddr= tcpSocket->GetClientaddr();
	int addrlen = sizeof(clientaddr);
	//클라이언트 정보 얻기
	getpeername(tcpSocket->GetClientSock(), (SOCKADDR*)&clientaddr, &addrlen);
	while (1) {
		tcpSocket->TcpSendData(index,tcpSocket->GetClientSock());
		//동기화 이벤트로 udp에 넘기는 부분
		tcpSocket->TcpRecvData();

		//동기화 이벤트로 main에 

	}

	// closesocket()
	closesocket(tcpSocket->GetClientSock());
	printf("\n[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(tcpSocket->GetClientaddr().sin_addr), ntohs(tcpSocket->GetClientaddr().sin_port));

	return 0;
}