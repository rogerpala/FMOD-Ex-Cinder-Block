#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "fmodexPlayer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class basicFmodExSoundPlayerApp : public AppBasic {
  public:
	void setup();
	void mouseMove( MouseEvent event );	
	void update();
	void draw();
	
	fmodexPlayer player;
	float speed;
	float pan;
	
	int circlePosY;
	
};

void basicFmodExSoundPlayerApp::setup()
{
	player.loadSound(getResourcePath("1085.mp3"));
	player.setVolume(0.75f);
	player.setLoop(true);
	player.play();
	
	circlePosY = 0;
	speed = 1.f;
	pan = 0.5f;
}

void basicFmodExSoundPlayerApp::mouseMove( MouseEvent event )
{
	pan = (float) event.getX() / getWindowWidth();
	player.setPan(pan);
	
	speed = 5 * ( (event.getY() - getWindowCenter().y) / getWindowHeight() ) + 1.f;
	player.setSpeed(speed);
}

void basicFmodExSoundPlayerApp::update()
{
	fmodexUpdate();
}

void basicFmodExSoundPlayerApp::draw()
{
	gl::clear( Color( .8f, .8f, .8f ) );
	gl::enableAlphaBlending();
	
	
	gl::color( ColorA( 126.f / 255.f, 29.f / 255.f, 246.f / 255.f, 1.f - pan ) );
	gl::drawSolidRect(Rectf(0, 0 , getWindowCenter().x, getWindowHeight()));
	gl::color( ColorA( 242.f / 255.f, 49.f / 255.f, 217.f / 255.f, pan ) );
	gl::drawSolidRect(Rectf(getWindowCenter().x, 0 , getWindowCenter().x * 2, getWindowHeight()));
	
	gl::color( ColorA( 42.f / 255.f, 234.f / 255.f, 187.f / 255.f, 1.f ) );
	circlePosY = circlePosY + speed * 2;
	if(circlePosY >= getWindowHeight()) circlePosY = 0;
	if(circlePosY < 0.f) circlePosY = getWindowHeight();
	gl::drawSolidCircle(Vec2f(getWindowCenter().x, circlePosY), 20.f);
	
	gl::color( Color( 0, 0, 0 ) );
	gl::drawString("pan: " + toString(pan), Vec2f(10.f, getWindowHeight() - 20.f));
	gl::drawString("speed: " + toString(speed), Vec2f(10.f, getWindowHeight() - 30.f));
	
}


CINDER_APP_BASIC( basicFmodExSoundPlayerApp, RendererGl )
