#pragma once
class CTCPsocket
{
	SOCKET m_listen_sock;
	SOCKADDR_IN m_serveraddr;

	SOCKET m_client_sock;
	SOCKADDR_IN m_clientaddr;

	CPlayer* m_pPlayer1;
	CPlayer* m_pPlayer2;
public:

	/// <summary>
	/// ������ ����� bind(), listen()���� �Ѵ�.
	/// </summary>
	CTCPsocket();
	CTCPsocket(CPlayer* p1, CPlayer* p2);
	virtual ~CTCPsocket();

	void TCPcommunications();
	SOCKET GetListenSock();
	SOCKET GetClientSock();

	SOCKADDR_IN GetServeraddr();
	SOCKADDR_IN GetClientaddr();

	void TcpSendData(int index, SOCKET socket);

	void TcpRecvData(int index, SOCKET socket);
};

DWORD WINAPI ProcessClient(CTCPsocket* arg);

