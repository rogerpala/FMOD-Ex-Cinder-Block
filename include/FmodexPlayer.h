/*
 *  FmodexPlayer.h
 *  
 *
 *  Created by Roger Palà on 21/03/2011.
 *  Copyright 2011 B-Reel. All rights reserved.
 *
 */

#include "fmod.hpp"
#include "fmod_errors.h"
#include "cinder/Utilities.h"
#include "cinder/app/AppBasic.h"

void FmodexStopAll();
void FmodexSetVolume(float vol);
void FmodexUpdate();						// calls FMOD update.
float * FmodexGetSpectrum(int nBands);		// max 512...


// --------------------- player functions:
class FmodexPlayer {
	
public:
	
	FmodexPlayer();
	virtual ~FmodexPlayer();
	
	void loadSound(std::string fileName, bool stream = false);
	void unloadSound();
	void play();
	void stop();
	
	void setVolume(float vol);
	void setPan(float vol);
	void setSpeed(float spd);
	void setPaused(bool bP);
	void setLoop(bool bLp);
	void setMultiPlay(bool bMp);
	void setPosition(float pct); // 0 = start, 1 = end;
	
	float getPosition();
	bool getIsPlaying();
	float getSpeed();
	float getPan();
	
	static void initializeFmod();
	static void closeFmod();
	
	bool isStreaming;
	bool bMultiPlay;
	bool bLoop;
	bool bLoadedOk;
	bool bPaused;
	float pan; // 0 - 1
	float volume; // 0 - 1
	float internalFreq; // 44100 ?
	float speed; // -n to n, 1 = normal, -1 backwards
	unsigned int length; // in samples;
	

	FMOD_RESULT result;
	FMOD::Channel * channel;
	FMOD::Sound * sound;
	

};

