#pragma once
class CTCPsocket
{
	SOCKET m_listen_sock;
	SOCKADDR_IN m_serveraddr;

	SOCKET m_client_sock;
	SOCKADDR_IN m_clientaddr;


public:

	/// <summary>
	/// ������ ����� bind(), listen()���� �Ѵ�.
	/// </summary>
	CTCPsocket();
	virtual ~CTCPsocket();

	void TCPcommunications();
};

DWORD WINAPI ProcessClient(LPVOID arg);