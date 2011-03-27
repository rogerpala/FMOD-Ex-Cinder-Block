/*
 *  Fmodex3DSoundPlayer.cpp
 *  
 *
 *  Created by Roger Palà on 21/03/2011.
 *  Copyright 2011 B-Reel. All rights reserved.
 *
 */

#include "Fmodex3DSoundPlayer.h"

bool bFmod3DInitialized = false;
static FMOD::ChannelGroup * channelGroup; 
static FMOD::System	* sys;

Fmodex3DSoundPlayer::Fmodex3DSoundPlayer(){
	initializeFmod();
}

// this should only be called once 
void Fmodex3DSoundPlayer::initializeFmod(){
	
	if(!bFmod3DInitialized){ 
		FMOD::System_Create(&sys); 
		sys->init(32, FMOD_INIT_NORMAL, NULL); //do we want just 32 channels?
		sys->set3DSettings(10.0f, 10.0f, 10.0f); 
		sys->getMasterChannelGroup(&channelGroup); 
		bFmod3DInitialized = true;
	}
}

FMOD::ChannelGroup * Fmodex3DSoundPlayer::getChannelGroup() {
	return channelGroup; 
}

FMOD::System * Fmodex3DSoundPlayer::getSystem() {
	return sys;
}

void Fmodex3DSoundPlayer::loadSound(std::string fileName, bool stream){ 
	result = sys->createSound(fileName.c_str(), FMOD_3D, NULL, &sound); 
	result = sound->set3DMinMaxDistance(1.f, 5000.0f);
	if (result != FMOD_OK){ 
		bLoadedOk = false; 
		printf("Fmodex3DSoundPlayer: Could not load sound file %s \n", fileName.c_str() ); 
	} else {
		bLoadedOk = true; 
		sound->getLength(&length, FMOD_TIMEUNIT_PCM); 
		isStreaming = stream;
	}
	
}

void Fmodex3DSoundPlayer::play(){
	
	sys->playSound(FMOD_CHANNEL_FREE, sound, bPaused, &channel); 
	FMOD_VECTOR pos = { 0.0f, 0.0f, 0.0f }; 
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f }; 
	channel->set3DAttributes(&pos, &vel); 
	channel->getFrequency(&internalFreq); 
	channel->setVolume(volume);
	
}




















