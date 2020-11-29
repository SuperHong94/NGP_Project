#include "stdafx.h"
#include "CPlayer.h"
CPlayer::CPlayer()
{
    m_pos.x = 0; m_pos.y = 0;
    m_useTeleport=false;
    m_telePos.x = 0; m_telePos.y = 0;
    m_useDash=false;
    m_hp=100;

}

CPlayer::~CPlayer()
{
}

myPOINT CPlayer::Getpos()
{
    return m_pos;
}

bool CPlayer::GetUseTeleport()
{
    return m_useTeleport;
}

myPOINT CPlayer::GetTelePos()
{
    return m_telePos;
}

bool CPlayer::GetuseDash()
{
    return m_useDash;
}

int CPlayer::GetHp()
{
   return m_hp;
}

void CPlayer::SetPos(myPOINT pos)
{
    m_pos = pos;
}


void CPlayer::SetUseTeleport(bool tele)
{
    m_useTeleport = tele;
}


void CPlayer::SetTelePos(myPOINT pos)
{
    m_telePos = pos;
}

void CPlayer::SetDash(bool dash)
{
    m_useDash;
}

void CPlayer::SetHP(int hp)
{
    m_hp = hp;
}
