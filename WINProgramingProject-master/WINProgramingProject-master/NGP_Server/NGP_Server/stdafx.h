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

enum EROUND {
	Round1,
	MAIN,
	HELP,
	Select,
	Round2,
	YouDie,
	YouWin,
	SelectPlay
};

typedef struct udpdata
{
	char type;
	char playerID;
	int playerXpos;
	int playerYpos;
	int collisionDamage;
	EROUND sceneState;
}udpData;

typedef struct tcpdata
{
	char type;
	char playerID;
	bool useTeleport;
	int teleportXpos;
	int teleportYpos;
	bool useDash;
	int hp;
}tcpData;