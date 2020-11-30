#include "stdafx.h"
#include "Utility.h"
#include "CTCPsocket.h"
#include "CPlayer.h"
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

	std::cout << "보내야 되는 데이터량: " << size << " 보낸 데이터량" << retval << ' ' << tcpData.playerID << "의 telX:" << tcpData.teleportXpos << "를 보냅니다.\n";
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


	cout << "데이터를 받습니다.\n";

	int retval = recv(sock, buffer, 512 - 1, 0);
	//index가 1이면 플레이어1이 보낸정보이다.
	buffer[retval] = '\0';

	tcpData = (tcpdata*)buffer;

	std::cout << "받은데이터량:  " << retval << ' ' << tcpData->playerID << "에서 받은 데이터: " << tcpData->teleportXpos << endl;
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



// 클라이언트와 데이터 통신 tcp
DWORD WINAPI ProcessClient(LPVOID arg)
{
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
	while (1) {
		cout << "recvEvent기다리기" << endl;
		if (index == 1)
			retval = WaitForSingleObject(g_hTCPRecvEvent, INFINITE);
		else if (index == 2) {
			retval = WaitForSingleObject(g_hTCPRecvEvent2, INFINITE);
		}
		cout << "tcprecv데이터 보내기 한다." << endl;
		tcpSocket->TcpSendData(index, client_sock);
		cout << "tcprecv데이터 보내기 완료." << endl;
		//tcp이벤트동기화 이벤트로 udp에 넘기는 부분
		//tcpSend완료 알림
		if (index == 1) {
			ResetEvent(g_hTCPRecvEvent);
			SetEvent(g_hTCPSendEvent);
		}
		else if (index == 2)
		{
			ResetEvent(g_hTCPRecvEvent2);
			SetEvent(g_hTCPSendEvent2);
		}




		//UDP데이터가 끝나기를 기다린다.
		if (index == 1)
			retval = WaitForSingleObject(g_hUDPEvent, INFINITE);
		else if (index == 2)
			retval = WaitForSingleObject(g_hUDPEvent2, INFINITE);
		cout << "tcprecv데이터 받기 한다." << endl;
		tcpSocket->TcpRecvData(index, client_sock);
		cout << "tcprecv데이터 받기 완료." << endl;
		//TCP리시브 받기 완료
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
	printf("\n[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(tcpSocket->GetClientaddr().sin_addr), ntohs(tcpSocket->GetClientaddr().sin_port));

	return 0;
}