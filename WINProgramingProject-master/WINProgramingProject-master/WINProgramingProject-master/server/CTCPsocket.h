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
	/// 대기소켓 만들고 bind(), listen()까지 한다.
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



