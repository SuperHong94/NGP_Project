//-----------------------------------해더파일 시작
#pragma once
#include "global.h"
#include <fmod.hpp>
#include <fmod_errors.h>
#pragma  comment( lib, "fmodex_vc.lib" )

enum ESOUNDKIND
{
	MainSound=0,
	Perion,
	STAGE2,
	YOUDIE,
	YOUWIN,
	SD_END,   //이펙트 끝 
};
enum EFFOUNDKIND //이펙트 사운드
{
	jump = 0,
	click = 1,
	teleP,
	Damage,
	EFFSD_END
};


void soundSetup();
void effSoundSetup();
void playSound(ESOUNDKIND eSound);
void effSoundoff();
void effPlaySound(EFFOUNDKIND eSound);
void SoundPause();