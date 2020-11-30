#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include<iomanip>




#define SERVERPORT 9000
#define BUFSIZE    512
using namespace std;


extern HANDLE g_hTCPSendEvent;
extern HANDLE g_hTCPRecvEvent;

extern HANDLE g_hUDPEvent;

extern char g_Id;
enum EROUND {
	Round1,
	MAIN,
	HELP,
	Select,
	Round2,
	YouDie,
	YouWin,
	SelectPlay,
	MultiReady
};

#pragma pack(1)
typedef struct udpdata
{
	char type;  //u면 udp데이터
	char playerID;
	int playerXpos;
	int playerYpos;
	EROUND sceneState;
}udpData;

typedef struct tcpdata
{
	char type; 
	char playerID;
	bool useTeleport;
	int teleportXpos;
	int teleportYpos;
	int playerXpos;
	int playerYpos;
	bool useDash;
	float hp;
}tcpData;
#pragma pack()