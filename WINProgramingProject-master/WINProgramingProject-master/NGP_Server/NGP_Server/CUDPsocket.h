#pragma once
class CPlayer;


class CUDPsocket
{
	SOCKET m_sock;
	SOCKADDR_IN m_serveraddr;

	SOCKET m_client_sock;
	SOCKADDR_IN m_clientaddr;

	CPlayer* m_pPlayer1;
	CPlayer* m_pPlayer2;
public:

	/// <summary>
	/// 대기소켓 만들고 bind(), listen()까지 한다.
	/// </summary>

	CUDPsocket();
	CUDPsocket(CPlayer* p1, CPlayer* p2);
	virtual ~CUDPsocket();

	void UDPSendData();

	void UDPRecvData();
};

