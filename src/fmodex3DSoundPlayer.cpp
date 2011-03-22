/*
 *  fmodex3DSoundPlayer.cpp
 *  basicApp
 *
 *  Created by Roger Palà on 21/03/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "fmodex3DSoundPlayer.h"

bool bFmod3DInitialized = false;
static FMOD_CHANNELGROUP * channelgroup; 
static FMOD_SYSTEM	* sys;

fmodex3DSoundPlayer::fmodex3DSoundPlayer(){
	initializeFmod();
}

// this should only be called once 
void fmodex3DSoundPlayer::initializeFmod(){
	
	if(!bFmod3DInitialized){ 
		FMOD_System_Create(&sys); 
		FMOD_System_Init(sys, 32, FMOD_INIT_NORMAL, NULL); //do we want just 32 channels?
		FMOD_System_Set3DSettings(sys, 10.0f, 10.0f, 10.0f); 
		FMOD_System_GetMasterChannelGroup(sys, &channelgroup); 
		bFmod3DInitialized = true;
	}
}

FMOD_CHANNELGROUP * fmodex3DSoundPlayer::getChannelGroup() {
	return channelgroup; 
}

FMOD_SYSTEM * fmodex3DSoundPlayer::getSystem() {
	return sys;
}

void fmodex3DSoundPlayer::loadSound(std::string fileName, bool stream){ 
	result = FMOD_System_CreateSound(sys, fileName.c_str(), FMOD_3D, NULL, &sound); 
	result = FMOD_Sound_Set3DMinMaxDistance(sound, 1.f, 5000.0f);
	if (result != FMOD_OK){ 
		bLoadedOk = false; 
		printf("ofSoundPlayer: Could not load sound file %s \n", fileName.c_str() ); 
	} else {
		bLoadedOk = true; 
		FMOD_Sound_GetLength(sound, &length, FMOD_TIMEUNIT_PCM); 
		isStreaming = stream;
	}
	
}

void fmodex3DSoundPlayer::play(){
	
	FMOD_System_PlaySound(sys, FMOD_CHANNEL_FREE, sound, bPaused, &channel); 
	FMOD_VECTOR pos = { 0.0f, 0.0f, 0.0f }; 
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f }; 
	FMOD_Channel_Set3DAttributes(channel, &pos, &vel); 
	FMOD_Channel_GetFrequency(channel, &internalFreq); 
	FMOD_Channel_SetVolume(channel,volume);
	
}




















