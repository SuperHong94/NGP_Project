#pragma once
class CPlayer;
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
	CTCPsocket(CPlayer* p1, CPlayer* p2);
	virtual ~CTCPsocket();

	SOCKET GetListenSock();
	SOCKET GetClientSock();

	SOCKADDR_IN GetServeraddr();
	SOCKADDR_IN GetClientaddr();

	SOCKET TCPAccept();
	int TcpSendData(int index, SOCKET socket);

	int TcpRecvData(int index, SOCKET socket);

	CPlayer* m_pPlayer1;
	CPlayer* m_pPlayer2;
};



