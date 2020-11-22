#include "stdafx.h"
#include "Utility.h"
#include "CTCPsocket.h"

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

}

CTCPsocket::~CTCPsocket()
{
}


// 클라이언트와 데이터 통신 tcp
DWORD WINAPI ProcessClient(LPVOID arg)
{

	CTCPsocket tcpSocket = (CTCPsocket)arg;

	while (1) {
		tcpSendData();


	}

	// closesocket()
	closesocket(client_sock);
	printf("\n[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}