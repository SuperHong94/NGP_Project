#pragma once
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

struct TCPDATA
{
	char type;
	char playerID;
	bool useTeleport;
	int teleportXpos;
	int teleportYpos;
	bool useDash;
	int hp;
};
#pragma pack()
struct myPOINT
{
	int x; int y;
};

class CPlayer
{
	myPOINT m_pos;
	bool m_useTeleport;
	myPOINT m_telePos;
	bool m_useDash;
	int m_hp;

	
public:
	CPlayer();
	virtual ~CPlayer();

	myPOINT Getpos();
	bool GetUseTeleport();
	myPOINT GetTelePos();
	bool GetuseDash();
	int GetHp();

	void SetPos(myPOINT pos);
	void SetUseTeleport(bool tele);
	void SetTelePos(myPOINT pos);
	void SetDash(bool dash);
	void SetHP(int hp); 

	EROUND m_state;
};

