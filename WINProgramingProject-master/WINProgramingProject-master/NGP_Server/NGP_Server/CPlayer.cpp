#include "CPlayer.h"

bool CPlayer::GetUseTeleport()
{
    return m_useTeleport;
}

POINT CPlayer::GetTelePos()
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
