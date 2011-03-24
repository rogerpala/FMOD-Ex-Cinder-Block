/*
 *  fmodex3DSoundPlayer.h
 *  
 *
 *  Created by Roger Palà on 21/03/2011.
 *  Copyright 2011 B-Reel. All rights reserved.
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
	static FMOD::ChannelGroup * getChannelGroup(); 
	static FMOD::System * getSystem();
}; 