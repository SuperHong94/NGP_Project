#pragma once

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

