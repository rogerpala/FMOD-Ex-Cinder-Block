/*
 *  fmodex3DSoundPlayer.h
 *  basicApp
 *
 *  Created by Roger Palà on 21/03/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "fmodexPlayer.h"

class fmodex3DSoundPlayer : public fmodexPlayer { 
public:
	fmodex3DSoundPlayer();
	static void initializeFmod(); 
	static void closeFmod(); 
	void loadSound(std::string fileName, bool stream = false); 
	void play(); 
	static FMOD_CHANNELGROUP * getChannelGroup(); 
	static FMOD_SYSTEM * getSystem();
}; 