/**
 * @author jonobr1 / http://jonobr1.com/
 *
 * Narcissus A light desktop application 
 * for creating visual connections of you 
 * and your actions.
 *
 * Creative Commons Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0)
 * http://creativecommons.org/licenses/by-sa/3.0/
 */


#include "cinder/app/AppBasic.h"
#include "cinder/qtime/MovieWriter.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Text.h"
#include "cinder/params/Params.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Rand.h"
#include "cinder/Font.h"
#include "boost/date_time.hpp"
#include "boost/date_time/local_time/local_time.hpp"
#include "cinder/gl/Fbo.h"
#include "cinder/cinderMath.h"

#include "Resources.h"

#include <iostream>
#include <fstream>
#include <time.h>
#include <sstream>
#include <sys/stat.h>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace boost::posix_time;
using namespace boost::local_time;


static const float		FRAMERATE = 30.0f;
static const int		WIDTH  = 640, HEIGHT = 480;
static const int		LERP_FACTOR = 4;

static const Area		CAPTURE_AREA = Area(0,0,WIDTH,HEIGHT);
static const Vec3f		CAPTURE_ROTATION = Vec3f( 180.0f, 180.0f, 0.0f );
static const Vec3f		CAPTURE_SCALE = Vec3f( 1.0f, 1.0f, 1.0f );
static const Vec2f		CAPTURE_POS = Vec2f(WIDTH, HEIGHT);
static const int		DESIRED_CAPTURE_WIDTH = WIDTH;
static const int		DESIRED_CAPTURE_HEIGHT = HEIGHT;
static const int		CV_CAPTURE_REDUCTION_FACTOR = 4; // both the capture width and height should be divisble by this number
static const Vec3f		CV_SCALE_TO_FIT_CAPTURE = Vec3f( CAPTURE_AREA.getWidth()/(float(DESIRED_CAPTURE_WIDTH)/float(CV_CAPTURE_REDUCTION_FACTOR)), CAPTURE_AREA.getWidth()/(float(DESIRED_CAPTURE_WIDTH)/float(CV_CAPTURE_REDUCTION_FACTOR)), 1 );

static const float		MAX_MISSED_CAPTURE_FRAMES = 150.0f;

static const int		WRITE_LOG_INTERVAL = 30;

static const int		MAX_ANCHORS = 100;
static const int		MAX_BLOBS	= 100;

static const int		DESIRED_ANCHORS = 7;
static const int		DESIRED_BLOBS	= 2;
static const int		DESIRED_HALOS	= 1;

static const float		COLORS[100][3] = {
	{0.016765479,0.87188804,0.6031046},
	{0.004004472,0.8077414,0.49074212},
	{0.03577246,0.9687631,0.6118954},
	{0.08925091,0.45855433,0.94268215},
	{0.66012704,0.05587073,0.070972875},
	{0.047585554,0.9926444,0.5653675},
	{0.49108025,0.034647625,0.20438343},
	{0.66433185,0.44972277,0.062040716},
	{0.52940345,0.04741929,0.2217578},
	{0.02899928,0.76545644,0.5943537},
	{0.66522884,0.064252615,0.27551928},
	{0.39944917,0.0624897,0.86926866},
	{0.3035826,0.032122474,0.8987097},
	{0.86204135,0.05511619,0.76137924},
	{0.16122247,0.017352223,0.62605804},
	{0.14915617,0.026256021,0.5208879},
	{0.3717165,0.04937169,0.8380306},
	{0.06633015,0.17312178,0.9073837},
	{0.32730925,0.028335426,0.6674054},
	{0.91929305,0.090118766,0.5437158},
	{0.86320186,0.5319982,0.02703932},
	{0.41829228,0.28133616,0.026704267},
	{0.5293567,0.031147841,0.32954168},
	{0.7459129,0.043166447,0.27040517},
	{0.03921276,0.61597836,0.6442821},
	{0.7530171,0.69750905,0.0012901709},
	{0.4751901,0.04640044,0.45712337},
	{0.6999527,0.41079485,0.03248929},
	{0.033737615,0.81728065,0.5271542},
	{0.72864324,0.7351202,0.022110984},
	{0.016767908,0.596416,0.62284976},
	{0.21916911,0.034956947,0.9290217},
	{0.8390871,0.06943208,0.26716146},
	{0.42894626,0.41449183,0.01027484},
	{0.7457916,0.04075054,0.36682168},
	{0.40628257,0.007031623,0.20101582},
	{0.575445,0.50570637,0.015562299},
	{0.6656817,0.0069730375,0.69403636},
	{0.19427966,0.0057219425,0.5080456},
	{0.9564649,0.06532838,0.8851621},
	{0.8618132,0.023001594,0.3483789},
	{0.2820651,0.03549587,0.6958275},
	{0.0330839,0.8483381,0.37545848},
	{0.37200683,0.017290944,0.7830873},
	{0.82695985,0.58430886,0.034980193},
	{0.08601287,0.49998355,0.033490807},
	{0.7781178,0.061926052,0.21507613},
	{0.89923257,0.07844292,0.20891905},
	{0.062903665,0.47948316,0.71913195},
	{0.18856825,0.018146815,0.4276993},
	{0.25776407,0.022112584,0.80832934},
	{0.23972014,0.024839625,0.6142174},
	{0.52464384,0.013376917,0.20321117},
	{0.04134275,0.79465026,0.99932325},
	{0.3249024,0.086718254,0.96977854},
	{0.39612466,0.039592266,0.9785031},
	{0.040698316,0.36548406,0.4187294},
	{0.024485996,0.11435648,0.84364116},
	{0.27117327,0.05173186,0.74193203},
	{.00029837352,0.39909145,0.43918908},
	{0.028644707,0.24987432,0.838501},
	{0.90016615,0.11849896,0.06561696},
	{0.03819399,0.3020684,0.5549964},
	{0.27551043,0.02179223,0.7084341},
	{0.33185562,0.062268216,0.717859},
	{0.075479984,0.042242527,0.5917764},
	{0.48011947,0.031733006,0.089534104},
	{0.20715839,0.026286727,0.7971479},
	{0.9741961,0.7980818,0.069171034},
	{0.59504706,0.08968946,0.019606471},
	{0.6918583,0.022975897,0.87255776},
	{0.5162983,0.02646111,0.0070801843},
	{0.686739,0.009635508,0.1574087},
	{0.002271445,0.05871846,0.58561826},
	{0.8826546,0.041760016,0.19941413},
	{0.10674816,0.03194436,0.69817615},
	{0.2721917,0.028830765,0.5563735},
	{0.15267234,0.015673025,0.8859432},
	{0.020135254,0.49947143,0.37773606},
	{0.73996294,0.06795667,0.73833895},
	{0.0071370904,0.6261521,0.78679335},
	{0.89276063,0.036489796,0.012724216},
	{0.12677085,0.005574402,0.54191697},
	{0.061709635,0.8219441,0.6531477},
	{0.4768204,0.07003082,0.031087484},
	{0.026415914,0.5116357,0.42977172},
	{0.043573037,0.5622177,0.50363666},
	{0.039887298,0.7027123,0.5977219},
	{0.08269887,0.96373653,0.76367545},
	{0.0067745354,0.10075171,0.7759115},
	{0.044627417,0.75431967,0.7765733},
	{0.5884261,0.053883724,0.9437045},
	{0.15684104,0.052363433,0.7074732},
	{0.5774164,0.054625463,0.029900128},
	{0.48784092,0.04434957,0.05004392},
	{0.565415,0.424869,0.016645676},
	{0.046399064,0.65550184,0.28970855},
	{0.9862708,0.012521251,0.1534688},
	{0.39270282,0.05574513,0.61509204},
	{0.88763154,0.065281585,0.08731247}
};


class NarcissusApp : public AppBasic {
public:
	void				prepareSettings( Settings *settings );
	void				setup();
	void				startCapture();
	void				passFullScreen();
	void				passRecording();
	void				keyDown( KeyEvent event );
	void				mouseDown( MouseEvent event );
	void				mouseMove( MouseEvent event );
	void				mouseDrag( MouseEvent event );
	void				update();
	void				draw();
	void				calibrateMovieWriter();
	void				initMovieWriter();
	void				renderInterface();
	void				updateInterface();
	
	void				reduceSurface( Surface * originalSurface, Surface * reducedSurface, int reduceFactor);
	void				brightnessContrastSurface(Surface *surface, Area area, float brightness=0, float contrast=0, int contrastPosition=128);
	void				compareSurface(Surface *surfaceA, Surface *surfaceB, int tolerance);
	bool*				comparedPixels;
	void				removeNoise( Surface *surface, int tolerance);
	
	Capture				capture;
	gl::Texture			captureTex;
	Surface8u			captureSurf;
	
	gl::Texture			cvTex;
	Surface8u			cvSurf, tempCvSurf, lastCvSurf;
	float				bright,contr,tol;
	bool				cleanNoise;
	Vec2i *				motionPoints;
	int					lastMotionPoint;
	Vec3f				screenScale;
	int					screenWidth;
	int					screenHeight;
	float				ratio;
	Font				verlag;
	TextLayout			calibration, noCamera;
	gl::Texture			information;
	float				gutter;
	
	int					blobsLength;
	int					blobLength;
	float				o;
	float				currentBlobs;
	float				currentAnchors;
	float				currentHalos;
	gl::Texture			halo;
	float				haloWidth;
	string				appPath;
	
	qtime::MovieWriter	mMovieWriter, cMovieWriter;
	bool				isRecording;
	gl::Fbo				imageOutput;
	Vec2f				screenTranslation;
	bool				isMouseMoving;
	int					mouseDelay;
	bool				initInterface;
	float				animationInc;
	float				beginning, end;
	Vec2f				mousePos;
	gl::Texture			record, toRecord, inRecord, checkDesktop;
	float				recordAnim;
	bool				isOverButton;
	float				desktopAnim;
	bool				isCalibrating;
	int					cDuration, cDelay;
	float				mMovieFrameRate;
	bool				isAddedBlending;
	
private:
	bool				cameraAvaliable;
	int					missedCaptureFrames;
	bool				ready;
	ofstream			logFile;
	
	bool				fullscreenAtStart;
	
};

void NarcissusApp::prepareSettings( Settings *settings ) {
	settings->setFrameRate( FRAMERATE );
	fullscreenAtStart = false;
	settings->setFullScreen( fullscreenAtStart );
	settings->setTitle("Narcissus");
	settings->setResizable( false );
}

void NarcissusApp::setup() {
	
	gl::clear( Color( 1.0, 1.0, 1.0 ), true );
	gl::color( Color( 1.0, 1.0, 1.0 ) );
	
	Settings appSettings = Settings();
	Display*  display    = appSettings.getDisplay();
	screenWidth  = display->getWidth();
	screenHeight = display->getHeight();
	if( screenWidth >= screenHeight )
		ratio = screenWidth / (float)WIDTH;
	else
		ratio = screenHeight / (float)HEIGHT;
	
	if( isFullScreen() ) {
		screenScale = Vec3f( ratio, ratio, 1.0 );
		hideCursor();
		if( screenWidth >= screenHeight )
			screenTranslation = Vec2f( CAPTURE_POS.x, CAPTURE_POS.y * ( screenHeight / (float) HEIGHT ) );
		else
			screenTranslation = Vec2f( CAPTURE_POS.x * ( screenWidth / (float) WIDTH ), CAPTURE_POS.y );
	}
	else {
		screenScale = CAPTURE_SCALE;
		screenTranslation = Vec2f( CAPTURE_POS.x, CAPTURE_POS.y );
		showCursor();
	}
	
	bright	= 0.0f;
	contr	= 0.0f;
	tol		= 100.0f;
	o		= 0.5f;
	cleanNoise = true;
	isRecording = false;
//	imageOutput = gl::Fbo( WIDTH, HEIGHT ); // MAC
//	imageOutput( 640, 480 );	// WINDOWS
	beginning = 0.0f;
	end = 1.0f;
	animationInc = 0.0f;
	recordAnim   = 0.0f;
	desktopAnim  = 0.0f;
	cDuration    = 0;
	cDelay		 = 0;
	mMovieFrameRate = FRAMERATE;
	isCalibrating	= true;
	isAddedBlending = true;
	
	haloWidth = 100.0f;
	halo			= gl::Texture( loadImage( loadResource( RES_HALO ) ) );
	toRecord		= gl::Texture( loadImage( loadResource( RES_RECORD ) ) );
	inRecord		= gl::Texture( loadImage( loadResource( RES_IN_RECORD ) ) );
	checkDesktop	= gl::Texture( loadImage( loadResource( RES_CHECK_DESK ) ) );
	record = toRecord;
	
	// Text to display to window
	verlag = Font( loadResource( RES_VERLAG ), 35.0f );
	calibration.setFont( verlag );
	gutter = 9.0f;
	calibration.setColor( ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );
	calibration.clear( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	calibration.addLine( "CALIBRATING CAMERA." );
	
	noCamera.setFont( verlag );
	noCamera.setColor( ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );
	noCamera.clear( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	noCamera.addLine( "UNABLE TO FIND WEBCAM." );
	noCamera.addLine( "IF NOT PLUGGED IN PLEASE PLUG ONE IN," );
	noCamera.addLine( "OTHERWISE UNPLUG AND PLUG IN AGAIN." );
	
	information = gl::Texture( calibration.render( true ) );
	
	
	currentBlobs	=(float)DESIRED_BLOBS;
	currentAnchors	=(float)DESIRED_ANCHORS;
	currentHalos    =(float)DESIRED_HALOS;
	screenScale     = CAPTURE_SCALE;
	
	startCapture();
	calibrateMovieWriter();
}

void NarcissusApp::startCapture() {
	
	ready = false;
	cameraAvaliable = false;
	missedCaptureFrames = 0;
	
	try {
		
		if(capture)capture.stop();	
		else capture = Capture(DESIRED_CAPTURE_WIDTH,DESIRED_CAPTURE_HEIGHT);
		capture.start();						
		
		captureTex = gl::Texture(capture.getWidth(),capture.getHeight());
		cvTex = gl::Texture(int(capture.getWidth()/CV_CAPTURE_REDUCTION_FACTOR),int(capture.getHeight()/CV_CAPTURE_REDUCTION_FACTOR));	
		cvSurf = Surface(int(capture.getWidth()/CV_CAPTURE_REDUCTION_FACTOR),int(capture.getHeight()/CV_CAPTURE_REDUCTION_FACTOR), false);
		comparedPixels = new bool[int(capture.getWidth()/CV_CAPTURE_REDUCTION_FACTOR)*int(capture.getHeight()/CV_CAPTURE_REDUCTION_FACTOR)];
		motionPoints = new Vec2i[int(capture.getWidth()/CV_CAPTURE_REDUCTION_FACTOR)*int(capture.getHeight()/CV_CAPTURE_REDUCTION_FACTOR)];
		
		cameraAvaliable=true;
		
		//console() << capture.getDevice().getUniqueId() << endl;
	}
	catch(CaptureExc &) {
		
		cameraAvaliable = false;
		//console() << "Unable to initialize device" << endl;
	}
}

void NarcissusApp::passRecording() {
	
	if( !isFullScreen() ) {
		isRecording = !isRecording;
		if( isRecording ) {
			initMovieWriter();
			record = inRecord;
		}
		else {
			mMovieWriter.finish();
			record = toRecord;
		}
		recordAnim = 0.5f;
	}
}

void NarcissusApp::passFullScreen() {
	
	if( !isRecording ) {
		setFullScreen( ! isFullScreen() );
		if( isFullScreen() ) {
			hideCursor();
			screenScale = Vec3f( ratio, ratio, 1.0 );
			if( screenWidth >= screenHeight )
				screenTranslation = Vec2f( CAPTURE_POS.x, CAPTURE_POS.y * ( screenHeight / (float) HEIGHT ) );
			else
				screenTranslation = Vec2f( CAPTURE_POS.x * ( screenWidth / (float) WIDTH ), CAPTURE_POS.y );
		}
		else {
			showCursor();
			screenScale = CAPTURE_SCALE;
			screenTranslation = Vec2f( CAPTURE_POS.x, CAPTURE_POS.y );
		}
	}
}

void NarcissusApp::keyDown( KeyEvent event ) {
	if( ( event.getChar() == ' ' || event.getCode() == app::KeyEvent::KEY_ESCAPE ) ) {
		passFullScreen();
		
	}
	if( ( event.getChar() == 'r' || event.getChar() == 'R' ) ) {
		passRecording();
	}
	if( ( event.getChar() == 'b' || event.getChar() == 'B' ) ) {
		isAddedBlending = !isAddedBlending;
		if(isAddedBlending) {
			o = 0.5f;
		} else {
			o = 1.0f;
		}
	}
}

void NarcissusApp::mouseDown( MouseEvent event ) {
	
	if( ( event.getX() > gutter && event.getX() < ( gutter + record.getWidth() ) ) &&
	   ( event.getY() > gutter && event.getY() < ( gutter + record.getHeight()) ) ) {
		passRecording();
		isOverButton = true;
	}
}

void NarcissusApp::mouseMove( MouseEvent event ) {
	isMouseMoving = true;
	if( ( event.getX() > gutter && event.getX() < ( gutter + record.getWidth() ) ) &&
	   ( event.getY() > gutter && event.getY() < ( gutter + record.getHeight()) ) )
		isOverButton = true;
	else isOverButton = false;
}

void NarcissusApp::mouseDrag( MouseEvent event ) {
    mouseMove( event );
}

void NarcissusApp::update() {
	if(cameraAvaliable) {
		if( capture && capture.checkNewFrame() ) {
			missedCaptureFrames=0;
			ready = true;
			captureSurf = capture.getSurface();
			captureTex.update(captureSurf);
			reduceSurface(&captureSurf,&cvSurf,CV_CAPTURE_REDUCTION_FACTOR);
			brightnessContrastSurface(&cvSurf,cvSurf.getBounds(),bright,contr);			
			tempCvSurf = cvSurf.clone();
			if(lastCvSurf)compareSurface(&cvSurf,&lastCvSurf,tol);
			lastCvSurf = tempCvSurf.clone();
			
			removeNoise(&cvSurf,0);
			
			cvTex.update(cvSurf);
		}
		else {
			missedCaptureFrames++;
			if(missedCaptureFrames>MAX_MISSED_CAPTURE_FRAMES) startCapture();
		}
	}
	else {
		missedCaptureFrames++;
		if(missedCaptureFrames>MAX_MISSED_CAPTURE_FRAMES) startCapture();
	}
	
	if( ready ) {
		
		imageOutput.bindFramebuffer();
		
		gl::pushModelView();
		gl::translate( screenTranslation );	
		gl::rotate( CAPTURE_ROTATION );	
		gl::draw( captureTex, CAPTURE_AREA );
		captureTex.disable();
		
		Vec3f colors[MAX_BLOBS];
		int	  randomIndices[MAX_BLOBS][MAX_ANCHORS];
		
		
		for(int i = 0; i < currentBlobs; i++) {
			colors[i] = Vec3f( Rand::randFloat( 0.0, 1.0 ), Rand::randFloat( 0.0, 1.0 ), Rand::randFloat( 0.0, 1.0 ) );
			for(int j = 0; j < currentAnchors; j++) {
				randomIndices[i][j] = Rand::randInt( lastMotionPoint );
			}
		}
		
		if( lastMotionPoint > 0 ) {
			
			// Draw the polygon
			gl::scale( CV_SCALE_TO_FIT_CAPTURE );
			
			if(isAddedBlending) gl::enableAdditiveBlending();
			else gl::enableAlphaBlending();
			
			for(int i = 0; i < currentBlobs; i++) {
				
				float r, g, b;
				
				if(isAddedBlending) {
					r = COLORS[Rand::randInt( 100 )][0];
					g = COLORS[Rand::randInt( 100 )][1];
					b = COLORS[Rand::randInt( 100 )][2];
				} else {
					r = 1.0f - COLORS[Rand::randInt( 100 )][0];
					g = 1.0f - COLORS[Rand::randInt( 100 )][1];
					b = 1.0f - COLORS[Rand::randInt( 100 )][2];
				}
				float q = ( Rand::randFloat() );
				
				gl::color( ColorA( r, g, b, o * q ) ) ;
				
				glBegin(GL_TRIANGLE_FAN);
				for(int j = 0; j < currentAnchors; j++) {
					gl::vertex( motionPoints[randomIndices[i][j]] );
				}
				glEnd();
			}
			
			// Draw the radial gradient
			for(int i = 0; i < currentHalos; i++) {
				float r, g, b;
				
				if(isAddedBlending) {
					r = COLORS[Rand::randInt( 100 )][0];
					g = COLORS[Rand::randInt( 100 )][1];
					b = COLORS[Rand::randInt( 100 )][2];
				} else {
					r = 1.0f - COLORS[Rand::randInt( 100 )][0];
					g = 1.0f - COLORS[Rand::randInt( 100 )][1];
					b = 1.0f - COLORS[Rand::randInt( 100 )][2];
				}
				
				int	  k = Rand::randInt( currentBlobs );
				int   j = Rand::randInt( currentAnchors );
				float a = ( Rand::randFloat() );
				float q = ( Rand::randFloat() );
				
				gl::color( ColorA( r, g, b, o * q ) ) ;
				gl::pushModelView();
				// Move halo in position
				gl::translate( motionPoints[randomIndices[k][j]] );
				// Scale randomly
				gl::scale( Vec3f( a, a, 1 ) );
				// Account for the width
				gl::translate( Vec2f( -( haloWidth )/2, -( haloWidth )/2 ));
				gl::draw( halo );
				halo.disable();
				gl::popModelView();
			}
			gl::disableAlphaBlending();
		}
		gl::popModelView();
		
		imageOutput.unbindFramebuffer();
	}
	
	updateInterface();
}

void NarcissusApp::draw() {
	
	gl::clear( Color( 1.0, 1.0, 1.0 ), true );
	gl::color( Color( 1.0, 1.0, 1.0 ) );
	
	// Record Information
	if( isCalibrating ) {
		
		if( cMovieWriter ) {
			
			gl::pushModelView();
			gl::translate( Vec2f( gutter, gutter ) );
			gl::draw( information );
			information.disable();
			gl::popModelView();
			
			cMovieWriter.addFrame( imageOutput.getTexture() );
			
			if( cDelay > FRAMERATE ) {
				
				mMovieFrameRate+=getAverageFps();
				
				if( cDuration < FRAMERATE ) cDuration++;
				else {
					isCalibrating = false;
					mMovieFrameRate = mMovieFrameRate / ( (float) cDuration );
					calibrateMovieWriter();
				}
			}
			cDelay++;
		}
	}
	else {
		
		if( ready )	{
			gl::pushModelView();
			gl::scale( screenScale );
			gl::draw( imageOutput.getTexture() );
			gl::popModelView();
			
			// Draw Button Interfaces
			if( !isFullScreen() )
				renderInterface();
		}
		else if( !cameraAvaliable || !ready ) {
			
			gl::pushModelView();
			gl::translate( Vec2f( gutter, gutter ) );
			gl::draw( information );
			information.disable();
			gl::popModelView();
		}
		
		if( isRecording ) {
			if( mMovieWriter ) {
				mMovieWriter.addFrame( imageOutput.getTexture() );
			}
		}
	}
}

void NarcissusApp::updateInterface() {
	if( !isOverButton ) {
		
		if( !isMouseMoving ) mouseDelay++;
		else mouseDelay = 0;
		
		if( mouseDelay > FRAMERATE ) initInterface = false;
		else initInterface = true;
	} else {
		initInterface = true;
	}
}

void NarcissusApp::renderInterface() {
	
	// Handle Opacity Math for Animating
	if( !isRecording ) {
		if( initInterface ) {
			if( animationInc >= 1.0f ) animationInc = 1.0f;
			else animationInc += .2f;
		}
		else {
			if( animationInc <= 0.0f ) animationInc = 0.0f;
			else animationInc -= .2f;
		}
	}
	else {
		animationInc = 1.0f;
	}
	
	if( recordAnim <= 0.0f ) recordAnim = 0.0f;
	else recordAnim -= .05f;
	
	if( !isRecording ) {
		if( recordAnim <= 0.0f ) {
			if( desktopAnim <= 0.0f ) desktopAnim = 0.0f;
			else desktopAnim -=.0125f;
		}
		else desktopAnim = 1.0f;
	}
	else desktopAnim = 0.0f;
	
	// Draw Interfaces
	gl::enableAlphaBlending();
	gl::pushModelView();
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, animationInc ) );
	gl::translate( Vec3f( gutter, gutter, 0.0f ) );
	gl::draw( record );
	record.disable();
	gl::pushModelView();
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, desktopAnim ) );
	gl::translate( Vec3f( record.getWidth() + gutter, 0.0f, 0.0f ) );
	gl::draw( checkDesktop );
	checkDesktop.disable();
	gl::popModelView();
	gl::popModelView();
	gl::pushModelView();
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, recordAnim) );
	glBegin( GL_QUADS );
	gl::vertex( Vec2f( 0.0f, 0.0f ) );
	gl::vertex( Vec2f( screenWidth, 0.0f ) );
	gl::vertex( Vec2f( screenWidth, screenHeight ) );
	gl::vertex( Vec2f( 0.0f, screenHeight ) );
	glEnd();
	gl::popModelView();
	gl::disableAlphaBlending();
	
	isMouseMoving = false;
}

void NarcissusApp::reduceSurface( Surface * originalSurface, Surface * reducedSurface, int reduceFactor) {
	
	Surface::Iter originalIter = originalSurface->getIter();
	Surface::Iter reducedIter = reducedSurface->getIter();
	
	int reduceFactorLimit = reduceFactor - 1;
	
	while( originalIter.line() && reducedIter.line()) {
		for (int i = 0; i<reduceFactorLimit; i++) originalIter.line();
		
		while( originalIter.pixel()&& reducedIter.pixel()) {
			for (int i = 0; i<reduceFactorLimit; i++) originalIter.pixel();
			
			reducedIter.r() = originalIter.r();
			reducedIter.g() = originalIter.g();
			reducedIter.b() = originalIter.b();
		}
	}
}

void NarcissusApp::brightnessContrastSurface( Surface *surface, Area area, float brightness, float contrast, int contrastPosition) {
	
	if(contrastPosition>255)contrastPosition=255;
	else if (contrastPosition<0)contrastPosition = 0;
	int multBrightness = brightness*255;
	Surface::Iter iter = surface->getIter( area );
	while( iter.line() ) {
		while( iter.pixel() ) {
			int rValue =  multBrightness + iter.r();
			rValue = int(float(rValue-contrastPosition)*contrast+float(rValue));
			if(rValue>255)rValue=255;
			else if (rValue<0)rValue = 0;
			iter.r() = rValue;
			int gValue =  multBrightness + iter.g();
			gValue = int(float(gValue-contrastPosition)*contrast+float(gValue));
			if(gValue>255)gValue=255;
			else if (gValue<0)gValue = 0;
			iter.g() = gValue;
			int bValue =  multBrightness + iter.b();
			bValue = int(float(bValue-contrastPosition)*contrast+float(bValue));
			if(bValue>255)bValue=255;
			else if (bValue<0)bValue = 0;
			iter.b() = bValue;
		}
	}
	
}

void NarcissusApp::compareSurface( Surface *surfaceA, Surface *surfaceB, int tolerance) {
	
	Surface::Iter iterA = surfaceA->getIter();
	Surface::Iter iterB = surfaceB->getIter();
	int pixelCount = 0;
	while( iterA.line() && iterB.line()) {
		while( iterA.pixel() && iterB.pixel()) {
			int value;
			if(	iterA.r()<=iterB.r()+tolerance
			   &&	iterA.r()>=iterB.r()-tolerance
			   &&	iterA.g()<=iterB.g()+tolerance
			   &&	iterA.g()>=iterB.g()-tolerance
			   &&	iterA.b()<=iterB.b()+tolerance
			   &&	iterA.b()>=iterB.b()-tolerance)
			{
				value=0;
				comparedPixels[pixelCount]=false;
			}
			else
			{
				value=255;
				comparedPixels[pixelCount]=true;
			}
			iterA.r() = iterA.g() = iterA.b() = value;
			pixelCount++;
		}
	}
}

void NarcissusApp::removeNoise( Surface *surface, int tolerance) {
	
	Surface::Iter iter = surface->getIter();
	
	int w = surface->getWidth();
	
	int pixelCount=0;
	int motionPointCount=0;
	
	while( iter.line()) {
		while( iter.pixel()) {
			if(iter.mY != 0 && iter.mX != 0 ) {
				if(comparedPixels[pixelCount-1]==false || comparedPixels[pixelCount-w]==false || comparedPixels[pixelCount-w-1]==false ) {
					iter.r() = iter.g() = iter.b() = 0;
				}
				else {
					motionPoints[motionPointCount]=Vec2i(iter.x(),iter.y());
					motionPointCount++;
				}
			}
			pixelCount++;
		}
	}
	
	lastMotionPoint = motionPointCount-1;
}

void NarcissusApp::calibrateMovieWriter() {
	if( isCalibrating ) {
		
		string path = getHomeDirectory() + "narcissusCalibration.mov";
		
		qtime::MovieWriter::Format format;
		format.setCodec( qtime::MovieWriter::CODEC_JPEG );
		format.setQuality( 0.6f );
		format.enableFrameTimeChanges( true );
		
		cMovieWriter = qtime::MovieWriter( path, WIDTH, HEIGHT, format );
	}
	else {
		cMovieWriter.finish();
		
		information = gl::Texture( noCamera.render( true ) );
		
		deleteFile( getHomeDirectory() + "narcissusCalibration.mov" );
	}
}

void NarcissusApp::initMovieWriter() {
	
	string pngPath = getHomeDirectory();
	
	local_date_time t = local_sec_clock::local_time(time_zone_ptr());
	
	string saveTime;
	stringstream out;
	out << t;
	saveTime = out.str();
	
	// Collate the following string for files
	for( int i = 0; i < 2; i++) {
		saveTime.replace(saveTime.find(":"), 1,"");
		saveTime.replace(saveTime.find(" "), 1, "_");
	}
	string path = pngPath + "Desktop/narcissus-" + saveTime + ".mov";
	
	qtime::MovieWriter::Format format;
	format.setCodec( qtime::MovieWriter::CODEC_JPEG );
	format.setQuality( 0.6f );
	format.setDefaultDuration( 1 / mMovieFrameRate );
	format.enableFrameTimeChanges( true );
	mMovieWriter = qtime::MovieWriter( path, WIDTH, HEIGHT, format );
}

CINDER_APP_BASIC( NarcissusApp, RendererGl )
