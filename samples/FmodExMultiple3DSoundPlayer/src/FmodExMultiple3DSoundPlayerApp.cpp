#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "Fmodex3DSoundPlayer.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "cinder/Rand.h"


using namespace ci;
using namespace ci::app;
using namespace std;

class FmodExMultiple3DSoundPlayerApp : public AppBasic {
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
	Fmodex3DSoundPlayer player1;
	Fmodex3DSoundPlayer player2;
	FMOD::System* sys;
	FMOD_VECTOR listenerVelocity; 
	FMOD_VECTOR listenerUp; 
	FMOD_VECTOR listenerForward; 
	FMOD_VECTOR listenerPos;
	
	FMOD_VECTOR	sound3DPosition1;
	FMOD_VECTOR	sound3DVelocity1;
	FMOD_VECTOR	sound3DPosition2;
	FMOD_VECTOR	sound3DVelocity2;
	
	float volume;
};


//arraw of sound paths
const std::string soundsPath[] = {
	"synth.wav",
	"1085.mp3"	
};

void FmodExMultiple3DSoundPlayerApp::setup()
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
	
	
	//we set the sounds positions
	Vec3f pos1 = Vec3f::zero() + 10 * Rand::randVec3f();
	sound3DPosition1.x = pos1.x;
	sound3DPosition1.y = pos1.y;
	sound3DPosition1.z = pos1.z;
	//we omit the sounds velocities for simplicity of the example
	sound3DVelocity1.x = 0;
	sound3DVelocity1.y = 0;
	sound3DVelocity1.z = 0;
	//we create the 3D sound players
	player1.loadSound(getResourcePath("synth.wav"));
	player1.setVolume(volume);
	player1.setMultiPlay(true); 
	player1.play();
	
	//we set the sounds positions
	Vec3f pos2 = Vec3f::zero() + 10 * Rand::randVec3f();
	sound3DPosition2.x = pos2.x;
	sound3DPosition2.y = pos2.y;
	sound3DPosition2.z = pos2.z;
	//we omit the sounds velocities for simplicity of the example
	sound3DVelocity2.x = 0;
	sound3DVelocity2.y = 0;
	sound3DVelocity2.z = 0;
	//we create the 3D sound players
	player2.loadSound(getResourcePath("1085.mp3"));
	player2.setVolume(volume);
	player2.setMultiPlay(true); 
	player2.play();
	
	
	sys = Fmodex3DSoundPlayer::getSystem();
	
	
	
	//Setup Params
	params = params::InterfaceGl( "3D_Sound", Vec2i( 200, 160 ) );
	params.addParam( "Scene Rotation", &sceneRotation, "" );
	params.addParam( "Light Direction", &lightDirection, "" );
	params.addSeparator();
	params.addParam( "Eye Distance", &camDistance, "min=-100.0 max=100.0 step=5.0 keyIncr=s keyDecr=w " );
	params.addParam( "Camera Center", &camCenter, "opened=1" );
	
}

void FmodExMultiple3DSoundPlayerApp::mouseDown( MouseEvent event )
{
	
}

void FmodExMultiple3DSoundPlayerApp::update()
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
	
	if(!player1.getIsPlaying())
		player1.play();
	
	if(!player2.getIsPlaying())
		player2.play();	
	
	player1.channel->set3DAttributes( &sound3DPosition1, &sound3DVelocity1);
	player2.channel->set3DAttributes( &sound3DPosition2, &sound3DVelocity2);
	
	//we update the fmod sound system
	sys->update();
	//FmodexUpdate();
	
}

void FmodExMultiple3DSoundPlayerApp::draw()
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
	
	//draw sound spheres
	Vec3f position1 = Vec3f(sound3DPosition1.x, sound3DPosition1.y, sound3DPosition1.z);
	gl::drawSphere(position1, .5f, 30.f);
	
	glMaterialfv( GL_FRONT, GL_DIFFUSE,	ColorA( 242.f / 255.f, 49.f / 255.f, 217.f / 255.f, 1.f ) );
	Vec3f position2 = Vec3f(sound3DPosition2.x, sound3DPosition2.y, sound3DPosition2.z);
	gl::drawSphere(position2, .5f, 30.f);
	
	
	glMaterialfv( GL_FRONT, GL_DIFFUSE,	ColorA( 1.f, 1.f, 1.f, .2f ) );
	glColor4f( ColorA( 1.0f, 1.0f, 1.0f, 0.1f ) );
	gl::drawSphere(position1, 6.f, 50.f);
	gl::drawSphere(position2, 6.f, 50.f);
	
	glDisable( GL_LIGHTING );
	glDisable( GL_LIGHT0 );
	gl::drawCoordinateFrame();
	
	//Draw Params
	params::InterfaceGl::draw();
	
}


CINDER_APP_BASIC( FmodExMultiple3DSoundPlayerApp, RendererGl )
