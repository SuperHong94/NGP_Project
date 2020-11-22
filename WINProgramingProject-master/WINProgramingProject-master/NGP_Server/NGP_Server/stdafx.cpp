#include "stdafx.h"


EROUND

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