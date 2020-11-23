#pragma once

struct POINT
{
	int x; int y;
};

class CPlayer
{
	POINT m_pos;
	bool m_useTeleport;
	POINT m_telePos;
	bool m_useDash;
	int m_hp;
public:
	CPlayer();
	virtual ~CPlayer();

	bool GetUseTeleport();
	POINT GetTelePos();
	bool GetuseDash();
	int GetHp();
};

