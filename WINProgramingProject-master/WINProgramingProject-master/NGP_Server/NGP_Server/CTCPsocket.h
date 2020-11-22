#pragma once
class CTCPsocket
{
	SOCKET m_listen_sock;
	SOCKADDR_IN m_serveraddr;

	SOCKET m_client_sock;
	SOCKADDR_IN m_clientaddr;


public:

	/// <summary>
	/// 대기소켓 만들고 bind(), listen()까지 한다.
	/// </summary>
	CTCPsocket();
	virtual ~CTCPsocket();

	void TCPcommunications();
};

DWORD WINAPI ProcessClient(LPVOID arg);