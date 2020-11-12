#include "SoundManager.h"

FMOD_SYSTEM* SJ_pSystem;
FMOD_SOUND* SJ_pSound[SD_END];
FMOD_CHANNEL* SJ_pChannel[SD_END];

FMOD_SYSTEM* effg_pSystem;  //
FMOD_SOUND* effg_pSound[EFFSD_END];
FMOD_CHANNEL* effg_pChannel[EFFSD_END];
int pause;
void soundSetup()
{
	char str[128];

	//사운드 시스템 생성
	FMOD_System_Create(&SJ_pSystem);

	//채널수, 모드, 0
	FMOD_System_Init(SJ_pSystem, 1, FMOD_INIT_NORMAL, NULL);

	//사운드 경로
	for (int i = 0; i < SD_END; i++) {
		wsprintfA(str, "sound\\STAGE%d.mp3", i);
		FMOD_System_CreateStream(SJ_pSystem, str, FMOD_LOOP_NORMAL, 0, &SJ_pSound[i]);
	}
	
}
void playSound(ESOUNDKIND eSound)
{
	FMOD_System_PlaySound(SJ_pSystem, FMOD_CHANNEL_FREE, SJ_pSound[eSound], 0, &SJ_pChannel[eSound]);

}

void effSoundSetup()
{
	char str[128];

	//사운드 시스템 생성
	FMOD_System_Create(&effg_pSystem);

	//채널수, 모드, 0
	FMOD_System_Init(effg_pSystem, 1, FMOD_INIT_NORMAL, NULL);

	//사운드 경로

	for (int i = 0; i < 4; i++) {
		wsprintfA(str, "effect\\effect%d.mp3", i);
		FMOD_System_CreateStream(effg_pSystem, str, FMOD_LOOP_OFF, 0, &effg_pSound[i]);
	}
}
void effSoundoff()
{
	for (int i = 0; i < EFFSD_END; i++)
		FMOD_Channel_Stop(effg_pChannel[i]);
}
void effPlaySound(EFFOUNDKIND eSound)
{
	FMOD_System_PlaySound(effg_pSystem, FMOD_CHANNEL_FREE, effg_pSound[eSound], 0, &effg_pChannel[eSound]);
}

void SoundPause()
{
	for (int i = 0; i < SD_END; i++)
		FMOD_Channel_GetPaused(SJ_pChannel[i],&pause );
	for (int i = 0; i < SD_END; i++)
		FMOD_Channel_SetPaused(SJ_pChannel[i], !pause);
}