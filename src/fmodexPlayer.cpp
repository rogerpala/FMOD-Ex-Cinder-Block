/*
 *  fmodexPlayer.cpp
 *  
 *
 *  Created by Roger Palà on 21/03/2011.
 *  Copyright 2011 B-Reel. All rights reserved.
 *
 */

#include "fmodexPlayer.h"

using namespace std;
using namespace cinder;
using namespace ci::app;


bool bFmodInitialized = false;
bool bUseSpectrum = false;
float fftValues[8192];			//
float fftInterpValues[8192];			//
float fftSpectrum[8192];		// maximum # is 8192, in fmodex....


// ---------------------  static vars
static FMOD::ChannelGroup * channelGroup;
static FMOD::System       * sys;

// these are global functions, that affect every sound / channel:
// ------------------------------------------------------------
// ------------------------------------------------------------

//--------------------
void fmodexStopAll(){
	fmodexPlayer::initializeFmod();
	channelGroup->stop();
}

//--------------------
void fmodexSetVolume(float vol){
	fmodexPlayer::initializeFmod();
	channelGroup->setVolume(vol);
}

//--------------------
void fmodexUpdate(){
	if (bFmodInitialized){
		sys->update();
	}
}

int ofNextPow2(int a){
	// from nehe.gamedev.net lesson 43
	int rval=1;
	while(rval<a) rval<<=1;
	return rval;
}

//--------------------
float * fmodexGetSpectrum(int nBands){
	
	fmodexPlayer::initializeFmod();
	
	
	// 	set to 0
	for (int i = 0; i < 8192; i++){
		fftInterpValues[i] = 0;
	}
	
	// 	check what the user wants vs. what we can do:
	if (nBands > 8192){
		console() << "error in fmodexGetSpectrum, the maximum number of bands is 8192 - you asked for" + toString(nBands) + "we will return 8192" << std::endl;
		nBands = 8192;
	} else if (nBands <= 0){
		console() << "error in fmodexSpectrum, you've asked for" + toString(nBands) + "bands, minimum is 1" << std::endl;
		return fftInterpValues;
	}
	
	// 	FMOD needs pow2
	int nBandsToGet = ofNextPow2(nBands);
	if (nBandsToGet < 64) nBandsToGet = 64;  // can't seem to get fft of 32, etc from fmodex
	
	// 	get the fft
	sys->getSpectrum(fftSpectrum, nBandsToGet, 0, FMOD_DSP_FFT_WINDOW_HANNING);
	
	// 	convert to db scale
	for(int i = 0; i < nBandsToGet; i++){
        fftValues[i] = 10.0f * (float)log10(1 + fftSpectrum[i]) * 2.0f;
	}
	
	// 	try to put all of the values (nBandsToGet) into (nBands)
	//  in a way which is accurate and preserves the data:
	//
	
	if (nBandsToGet == nBands){
		
		for(int i = 0; i < nBandsToGet; i++){
			fftInterpValues[i] = fftValues[i];
		}
		
	} else {
		
		float step 		= (float)nBandsToGet / (float)nBands;
		//float pos 		= 0;
		// so for example, if nBands = 33, nBandsToGet = 64, step = 1.93f;
		int currentBand = 0;
		
		for(int i = 0; i < nBandsToGet; i++){
			
			// if I am current band = 0, I care about (0+1) * step, my end pos
			// if i > endPos, then split i with me and my neighbor
			
			if (i >= ((currentBand+1)*step)){
				
				// do some fractional thing here...
				float fraction = ((currentBand+1)*step) - (i-1);
				float one_m_fraction = 1 - fraction;
				fftInterpValues[currentBand] += fraction * fftValues[i];
				currentBand++;
				// safety check:
				if (currentBand >= nBands){
					console() << "fmodexGetSpectrum - currentBand >= nBands" << std::endl;
				}
				
				fftInterpValues[currentBand] += one_m_fraction * fftValues[i];
				
			} else {
				// do normal things
				fftInterpValues[currentBand] += fftValues[i];
			}
		}
		
		// because we added "step" amount per band, divide to get the mean:
		for (int i = 0; i < nBands; i++){
			fftInterpValues[i] /= step;
			if (fftInterpValues[i] > 1)fftInterpValues[i] = 1; 	// this seems "wrong"
		}
		
	}
	
	return fftInterpValues;
}

// ------------------------------------------------------------
// ------------------------------------------------------------


// now, the individual sound player:
//------------------------------------------------------------
fmodexPlayer::fmodexPlayer(){
	bLoop 			= false;
	bLoadedOk 		= false;
	pan 			= 0.5f;
	volume 			= 1.0f;
	internalFreq 	= 44100;
	speed 			= 1;
	bPaused 		= false;
	isStreaming		= false;
}

fmodexPlayer::~fmodexPlayer(){
	unloadSound();
}



//---------------------------------------
// this should only be called once
void fmodexPlayer::initializeFmod(){
	if(!bFmodInitialized){
		FMOD::System_Create(&sys);
#ifdef TARGET_LINUX
		sys->setOutput(FMOD_OUTPUTTYPE_ALSA);
#endif
		sys->init(32, FMOD_INIT_NORMAL, NULL);  //do we want just 32 channels?
		sys->getMasterChannelGroup(&channelGroup);
		bFmodInitialized = true;
	}
}


//---------------------------------------
void fmodexPlayer::closeFmod(){
	if(bFmodInitialized){
		sys->close();
		bFmodInitialized = false;
	}
}

//------------------------------------------------------------
void fmodexPlayer::loadSound(std::string fileName, bool stream){
	
	
	// fmod uses IO posix internally, might have trouble
	// with unicode paths...
	// says this code:
	// http://66.102.9.104/search?q=cache:LM47mq8hytwJ:www.cleeker.com/doxygen/audioengine__fmod_8cpp-source.html+FSOUND_Sample_Load+cpp&hl=en&ct=clnk&cd=18&client=firefox-a
	// for now we use FMODs way, but we could switch if
	// there are problems:
	
	bMultiPlay = false;
	
	// [1] init fmod, if necessary
	
	initializeFmod();
	
	// [2] try to unload any previously loaded sounds
	// & prevent user-created memory leaks
	// if they call "loadSound" repeatedly, for example
	
	unloadSound();
	
	// [3] load sound
	
	//choose if we want streaming
	int fmodFlags =  FMOD_SOFTWARE;
	if(stream)fmodFlags =  FMOD_SOFTWARE | FMOD_CREATESTREAM;
	
	result = sys->createSound(fileName.c_str(),  fmodFlags, NULL, &sound);
	
	if (result != FMOD_OK){
		bLoadedOk = false;
		console() << "fmodexPlayer: Could not load sound file " + fileName << std::endl;
	} else {
		bLoadedOk = true;
		sound->getLength(&length, FMOD_TIMEUNIT_PCM);
		isStreaming = stream;
	}
	
}

//------------------------------------------------------------
void fmodexPlayer::unloadSound(){
	if (bLoadedOk){
		stop();						// try to stop the sound
		if(!isStreaming) sound->release();
	}
}

//------------------------------------------------------------
bool fmodexPlayer::getIsPlaying(){
	
	if (!bLoadedOk) return false;
	
	bool playing;
	channel->isPlaying(&playing);
	return playing;
}

//------------------------------------------------------------
float fmodexPlayer::getSpeed(){
	return speed;
}

//------------------------------------------------------------
float fmodexPlayer::getPan(){
	return pan;
}

//------------------------------------------------------------
void fmodexPlayer::setVolume(float vol){
	if (getIsPlaying() == true){
		channel->setVolume(vol);
	}
	volume = vol;
}

//------------------------------------------------------------
void fmodexPlayer::setPosition(float pct){
	if (getIsPlaying() == true){
		int sampleToBeAt = (int)(length * pct);
		channel->setPosition(sampleToBeAt, FMOD_TIMEUNIT_PCM);
	}
}

//------------------------------------------------------------
float fmodexPlayer::getPosition(){
	if (getIsPlaying() == true){
		unsigned int sampleImAt;
		
		channel->getPosition(&sampleImAt, FMOD_TIMEUNIT_PCM);
		
		float pct = 0.0f;
		if (length > 0){
			pct = sampleImAt / (float)length;
		}
		return pct;
	} else {
		return 0;
	}
}

//------------------------------------------------------------
void fmodexPlayer::setPan(float p){
	if (getIsPlaying() == true){
		channel->setPan(p);
	}
	pan = p;
}


//------------------------------------------------------------
void fmodexPlayer::setPaused(bool bP){
	if (getIsPlaying() == true){
		channel->setPaused(bP);
		bPaused = bP;
	}
}


//------------------------------------------------------------
void fmodexPlayer::setSpeed(float spd){
	if (getIsPlaying() == true){
		channel->setFrequency( internalFreq * spd);
	}
	speed = spd;
}


//------------------------------------------------------------
void fmodexPlayer::setLoop(bool bLp){
	if (getIsPlaying() == true){
		channel->setMode( (bLp == true) ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	}
	bLoop = bLp;
}

// ----------------------------------------------------------------------------
void fmodexPlayer::setMultiPlay(bool bMp){
	bMultiPlay = bMp;		// be careful with this...
}

// ----------------------------------------------------------------------------
void fmodexPlayer::play(){
	
	// if it's a looping sound, we should try to kill it, no?
	// or else people will have orphan channels that are looping
	if (bLoop == true){
		channel->stop();
	}
	
	// if the sound is not set to multiplay, then stop the current,
	// before we start another
	if (!bMultiPlay){
		channel->stop();
	}
	
	sys->playSound(FMOD_CHANNEL_FREE, sound, bPaused, &channel);
	
	channel->getFrequency(&internalFreq);
	channel->setVolume(volume);
	channel->setPan(pan);
	channel->setFrequency(internalFreq * speed);
	channel->setMode((bLoop == true) ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	
	//fmod update() should be called every frame - according to the docs.
	//we have been using fmod without calling it at all which resulted in channels not being able
	//to be reused.  we should have some sort of global update function but putting it here
	//solves the channel bug
	sys->update();
	
}

// ----------------------------------------------------------------------------
void fmodexPlayer::stop(){
	channel->stop();
}
