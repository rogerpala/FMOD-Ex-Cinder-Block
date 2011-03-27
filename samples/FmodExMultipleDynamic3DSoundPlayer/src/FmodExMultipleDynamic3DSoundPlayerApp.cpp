#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "Fmodex3DSoundPlayer.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "cinder/Rand.h"
#include <vector>


using namespace ci;
using namespace ci::app;
using namespace std;

class FmodExMultipleDynamic3DSoundPlayerApp : public AppBasic {
public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	
	//3D Scene
	params::InterfaceGl	params;
	CameraPersp cam;
	Quatf sceneRotation;
	float camDistance;
	Vec3f camEye, camCenter, camUp;
	Vec3f lightDirection;
	Vec3f forward;
	Vec3f camInitialEye;
	
	//Multiple 3D Sound
	std::vector<Fmodex3DSoundPlayer*> players;
	Fmodex3DSoundPlayer player2;
	FMOD::System* sys;
	FMOD_VECTOR listenerVelocity; 
	FMOD_VECTOR listenerUp; 
	FMOD_VECTOR listenerForward; 
	FMOD_VECTOR listenerPos;
	
	std::vector<FMOD_VECTOR>	sound3DPositions;
	std::vector<FMOD_VECTOR>	sound3DVelocities;
	
	float volume;
};


//array of sound paths
const std::string soundsPath[] = {
	"synth.wav",
	"1085.mp3",
	"synth.wav",
	"1085.mp3",
	"synth.wav",
	"1085.mp3"
};

void FmodExMultipleDynamic3DSoundPlayerApp::setup()
{
	setWindowSize(1024, 768);
	//Setup camera
	camDistance = 0.0f;
	camEye			= Vec3f( 0.0f, 0.0f, 20.f );
	camInitialEye = camEye;
	camCenter			= Vec3f(0, 0, -1.f);
	camUp				= Vec3f::yAxis();
	cam.setPerspective( 75.0f, getWindowAspectRatio(), 1.0f, 2000.0f );
	cam.lookAt( camEye, camCenter, camUp );
	
	//Setup Light
	lightDirection = Vec3f( 0, 0, -1 );
	
	
	//Setup 3D Sounds
	volume = 1.f;
	//we omit the velocities for simplicity of the example
	listenerVelocity.x = 0; 
	listenerVelocity.y = 0; 
	listenerVelocity.z = 0; 
	//we setup the listener with the same propierties of the camera (first person view)
	listenerUp.x = camUp.x;
	listenerUp.y = camUp.y; 
	listenerUp.z = camUp.z;
	listenerPos.x = camEye.x; 
	listenerPos.y = camEye.y;  
	listenerPos.z = camEye.z;
	forward = camCenter - camEye;
	forward.normalize();
	listenerForward.x = forward.x; 
	listenerForward.y = forward.y; 
	listenerForward.z = forward.z; 
	
	int numberOfSounds = sizeof(soundsPath) / sizeof(std::string);
	console() << "numberOfSounds: " << numberOfSounds << std::endl;
	for (int i = 0; i < numberOfSounds; i++) {
		//we set the sounds positions
		Vec3f pos1 = Vec3f::zero() + 30 * Rand::randVec3f();
		FMOD_VECTOR position;
		position.x = pos1.x;
		position.y = pos1.y;
		position.z = pos1.z;
		sound3DPositions.push_back(position);
		//we omit the sounds velocities for simplicity of the example
		FMOD_VECTOR velocity;
		velocity.x = 0;
		velocity.y = 0;
		velocity.z = 0;
		sound3DVelocities.push_back(velocity);
		//we create the 3D sound players
		Fmodex3DSoundPlayer* player = new Fmodex3DSoundPlayer();
		player->loadSound(getResourcePath(soundsPath[i]));
		player->setVolume(volume);
		player->setMultiPlay(true); 
		player->play();
		players.push_back(player);
	}
		
	

	
	
	sys = Fmodex3DSoundPlayer::getSystem();
	
	
	
	//Setup Params
	params = params::InterfaceGl( "3D_Sound", Vec2i( 200, 160 ) );
	params.addParam( "Scene Rotation", &sceneRotation, "" );
	params.addParam( "Light Direction", &lightDirection, "" );
	params.addSeparator();
	params.addParam( "Eye Distance", &camDistance, "min=-100.0 max=100.0 step=5.0 keyIncr=s keyDecr=w " );
	params.addParam( "Camera Center", &camCenter, "opened=1" );
	
}

void FmodExMultipleDynamic3DSoundPlayerApp::mouseDown( MouseEvent event )
{
	
}

void FmodExMultipleDynamic3DSoundPlayerApp::update()
{
	
	
	//Update camera
	Vec3f camDirection = camCenter;
	camDirection.normalize();
	camEye = camInitialEye + camDistance * camDirection;
	cam.setViewDirection(camCenter);
	cam.setEyePoint(camEye);
	gl::setMatrices( cam );
	gl::rotate( sceneRotation );
	
	//Update 3D Sounds
	
	Vec3f up = cam.getWorldUp();
	up.invert();
	
	listenerUp.x = up.x;
	listenerUp.y = up.y; 
	listenerUp.z = up.z;
	listenerPos.x = camEye.x; 
	listenerPos.y = camEye.y;  
	listenerPos.z = camEye.z;
	//forward = camCenter - camEye;
	//forward.normalize();
	listenerForward.x = camCenter.x; 
	listenerForward.y = camCenter.y; 
	listenerForward.z = camCenter.z;
	sys->set3DListenerAttributes( 0, &listenerPos, &listenerVelocity, &listenerForward, &listenerUp);
	
	for (int i = 0; i < players.size(); i++) {
		//we play the sound if it's not playing (loop)
		if(!(players[i]->getIsPlaying())) {
			console() << "play " << std::endl;
			players[i]->play();
		}
		//we pass the listener propierties and the sound propierties to the fmod system and channel
		players[i]->channel->set3DAttributes( &sound3DPositions[i], &sound3DVelocities[i]);
	}
	
	//we update the fmod sound system
	sys->update();
	//FmodexUpdate();
	
}

void FmodExMultipleDynamic3DSoundPlayerApp::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.2f ), true );
	gl::enableAlphaBlending();
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	//Draw 3D Scene
	glColor4f( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	
	//setup Lighting
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );	
	GLfloat lightPosition[] = { -lightDirection.x, -lightDirection.y, -lightDirection.z, 0.0f };
	glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );
	glMaterialfv( GL_FRONT, GL_DIFFUSE,	ColorA( 42.f / 255.f, 234.f / 255.f, 187.f / 255.f, 1.f ) );
	
	
	for (int i = 0; i < sound3DPositions.size(); i++) {
		Vec3f position = Vec3f(sound3DPositions[i].x, sound3DPositions[i].y, sound3DPositions[i].z);
		gl::drawSphere(position, .5f, 30.f);
	}
	
	glMaterialfv( GL_FRONT, GL_DIFFUSE,	ColorA( 1.f, 1.f, 1.f, .2f ) );
	glColor4f( ColorA( 1.0f, 1.0f, 1.0f, 0.1f ) );
	for (int i = 0; i < sound3DPositions.size(); i++) {
		Vec3f position = Vec3f(sound3DPositions[i].x, sound3DPositions[i].y, sound3DPositions[i].z);
		gl::drawSphere(position, 6.f, 50.f);
	}
	
	glDisable( GL_LIGHTING );
	glDisable( GL_LIGHT0 );
	gl::drawCoordinateFrame();
	
	//Draw Params
	params::InterfaceGl::draw();
	
}


CINDER_APP_BASIC( FmodExMultipleDynamic3DSoundPlayerApp, RendererGl )
