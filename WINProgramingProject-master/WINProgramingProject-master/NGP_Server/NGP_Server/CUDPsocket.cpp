#include "stdafx.h"
#include "CUDPsocket.h"
#include "Utility.h"
#include "CPlayer.h"

CUDPsocket::CUDPsocket()
{
}

CUDPsocket::CUDPsocket(CPlayer* p1, CPlayer* p2)
{
	int retval;
	// socket()
	m_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sock == INVALID_SOCKET) err_quit("socket()");

	//브로드캐스트 활성화
	BOOL bEnable = TRUE;
	retval = setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST,
		(char*)&bEnable, sizeof(bEnable));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");



	// bind()
	ZeroMemory(&m_serveraddr, sizeof(m_serveraddr));
	m_serveraddr.sin_family = AF_INET;
	m_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(m_sock, (SOCKADDR*)&m_serveraddr, sizeof(m_serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");
}

CUDPsocket::~CUDPsocket()
{
}

void CUDPsocket::UDPSendData()
{
	int retval;

	udpData uData;
	myPOINT postion;
	postion = m_pPlayer1->Getpos();
	uData.collisionDamage = 0;
	uData.playerID = '1';
	uData.playerXpos = postion.x;
	uData.playerYpos = postion.y;
	uData.sceneState = m_pPlayer1->m_state;

	int size = sizeof(udpData);
	//player1정보 보내기
	retval = sendto(m_sock, (char*)&uData, size, 0,
		(SOCKADDR*)&m_clientaddr, sizeof(m_clientaddr));
	if (retval == SOCKET_ERROR) {
		err_display("sendto()");
	}

	postion = m_pPlayer2->Getpos();
	uData.collisionDamage = 0;
	uData.playerID = '2';
	uData.playerXpos = postion.x;
	uData.playerYpos = postion.y;
	uData.sceneState = m_pPlayer2->m_state;

	//player2정보 보내기
	retval = sendto(m_sock, (char*)&uData, size, 0,
		(SOCKADDR*)&m_clientaddr, sizeof(m_clientaddr));
	if (retval == SOCKET_ERROR) {
		err_display("sendto()");
	}
}

void CUDPsocket::UDPRecvData()
{
	int retval;
	
	udpData* uData;
	myPOINT postion;
	char buffer[512];
	int addresslen = sizeof(m_clientaddr);
	int hp = 0;
	retval = recvfrom(m_sock, buffer, 512-1, 0,
		(SOCKADDR*)&m_clientaddr, &addresslen);

	buffer[retval] = '\0';

	uData = (udpData*)buffer;

	if (retval == SOCKET_ERROR) {
		err_display("recvfrom()");
	}
	switch (uData->playerID)
	{
	case '1':
		hp = m_pPlayer1->GetHp();
		postion.x = uData->playerXpos;
		postion.y = uData->playerYpos;
		m_pPlayer1->SetPos(postion);
		m_pPlayer1->m_state = uData->sceneState;
		m_pPlayer1->SetHP(hp - uData->collisionDamage); 
		break;
	case '2':
		hp = m_pPlayer2->GetHp();
		postion.x = uData->playerXpos;
		postion.y = uData->playerYpos;
		m_pPlayer2->SetPos(postion);
		m_pPlayer2->m_state = uData->sceneState;
		m_pPlayer2->SetHP(hp - uData->collisionDamage);
		break;
	default:
		break;
	}
}
