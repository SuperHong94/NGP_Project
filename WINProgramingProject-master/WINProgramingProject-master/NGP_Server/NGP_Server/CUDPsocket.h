#pragma once
class CPlayer;


class CUDPsocket
{
	SOCKET m_sock;
	SOCKADDR_IN m_serveraddr;

	SOCKET m_client_sock;
	SOCKADDR_IN m_clientaddr;

	
public:

	/// <summary>
	/// ������ ����� bind(), listen()���� �Ѵ�.
	/// </summary>
	
	CUDPsocket();
	CUDPsocket(CPlayer* p1, CPlayer* p2);
	virtual ~CUDPsocket();

	void UDPSendData(int index);
	void UDPSendData();

	void UDPRecvData(int index);
	void UDPRecvData();

	CPlayer* m_pPlayer1;
	CPlayer* m_pPlayer2;
};

