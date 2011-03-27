/*
 *  
 *
 *
 *  Created by Roger Palà on 21/03/2011.
 *  Copyright 2011 B-Reel. All rights reserved.
 *
 */



#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "Fmodex3DSoundPlayer.h"

#define SCALE 400.f

using namespace ci;
using namespace ci::app;
using namespace std;

class FmodEx3DSoundPlayerApp : public AppBasic {
  public:
	void setup();
	void mouseMove( MouseEvent event );	
	void update();
	void draw();
	
	Fmodex3DSoundPlayer player;
	FMOD::System *sys;
	FMOD_VECTOR listenerVelocity; 
	FMOD_VECTOR listenerUp; 
	FMOD_VECTOR listenerForward; 
	FMOD_VECTOR listenerPos;
	FMOD_VECTOR soundPosition; 
	FMOD_VECTOR soundVelocity;
};

void FmodEx3DSoundPlayerApp::setup()
{
	setWindowSize(1024, 768);
	
	//we are not going to consider listener or sound velocity in this example, we set them to zero
	listenerVelocity.x = 0; 
	listenerVelocity.y = 0; 
	listenerVelocity.z = 0;
	soundVelocity.x = 0; 
	soundVelocity.y = 0; 
	soundVelocity.z = 0.0;
	
	//we set the listenerUp and the listenerForward vectors to face the listener in the -y axis direction
	listenerUp.x = 0.f;
	listenerUp.y = 0.f; 
	listenerUp.z = 1.f; 
	listenerForward.x = 0.f; 
	listenerForward.y = -1.f; 
	listenerForward.z = 0.0;
	
	//we set the listener position on the middle of the screen
	listenerPos.x = getWindowWidth() * 0.5f / SCALE; 
	listenerPos.y = getWindowHeight() * 0.5f / SCALE;  
	listenerPos.z = 1.f; 
	
	//we set the initial sound position 
	soundPosition.x = getWindowWidth() * 0.8f / SCALE;
	soundPosition.y = getWindowHeight() * 0.8f / SCALE;
	soundPosition.z = 1.0; 
	
	//we load the sound and start playing it
	player.loadSound(getResourcePath("synth.wav")); 
	player.setVolume(1.f); 
	player.play();
	
	
	sys = Fmodex3DSoundPlayer::getSystem();
}

void FmodEx3DSoundPlayerApp::mouseMove( MouseEvent event )
{
	soundPosition.x = event.getX() / SCALE;
	soundPosition.y = event.getY() / SCALE;
}

void FmodEx3DSoundPlayerApp::update()
{
	//we play the sound if it's not playing (loop)
	if(!player.getIsPlaying())
		player.play();
	
	
	//we pass the listener propierties and the sound propierties to the fmod system and channel
	sys->set3DListenerAttributes( 0, &listenerPos, &listenerVelocity, &listenerForward, &listenerUp);
	player.channel->set3DAttributes( &soundPosition, &soundVelocity);
	//we update the fmod sound system
	sys->update();
}

void FmodEx3DSoundPlayerApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0.8f, 0.8f, 0.8f ) ); 
	
	float radius = 10 + soundPosition.z * 10;
	gl::color( ColorA( 126.f / 255.f, 29.f / 255.f, 246.f / 255.f, 1.f ) );
	gl::drawSolidCircle(Vec2f(soundPosition.x * SCALE, soundPosition.y * SCALE) , radius );
	
	gl::color( ColorA( 242.f / 255.f, 49.f / 255.f, 217.f / 255.f, 1.f ) );
	radius = 10 + listenerPos.z * 10;
	gl::drawSolidCircle(Vec2f(listenerPos.x * SCALE, listenerPos.y * SCALE) , radius);

}


CINDER_APP_BASIC( FmodEx3DSoundPlayerApp, RendererGl )
