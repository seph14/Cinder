// This sample demonstrates the use of custom callbacks for tween start, update, and completion
// * The start callback sets the background green using a functor
// * The completion callback sets the background to blue using a free function
// * The update callback makes the radius of the circle the distance to the nearest edge using a member function

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#if defined( CINDER_GLES2 )
#include "cinder/gl/gles2.h"
#endif

#include "cinder/Timeline.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// global variable for background color
Color	gBackgroundColor;

struct Circle {
	void posUpdate() { // make the radius the distance to the closest edge
		mRadius = mPos().x;
		mRadius = std::min( mRadius, getWindowWidth() - mPos().x );
		mRadius = std::min( mRadius, mPos().y );
		mRadius = std::min( mRadius, getWindowHeight() - mPos().y );		
	}

	void draw() {
		gl::color( Color( 1.0f, 0.5f, 0.25f ) );
		gl::drawSolidCircle( mPos, mRadius );
	}
	
	Anim<Vec2f>	mPos;
	float		mRadius;
};

// Functor which sets the Color pointed to by colorPtr to green
struct ColorToGreenFunctor {
	ColorToGreenFunctor( Color *colorPtr )
		: mColorPtr( colorPtr )
	{}
	
	void operator()() {
		*mColorPtr = Color( 0.5f, 0.9f, 0.5f );
	}
	
	Color		*mColorPtr;
};

class CustomCallbackApp : public AppNative {
  public:
    void prepareSettings(Settings *settings);
	void setup();
	void mouseDown( MouseEvent event );	
	void draw();
	
	Circle		mCircle;

#if defined( CINDER_GLES2 )
    gl::GlesContextRef mContext;
#endif
};

void CustomCallbackApp::prepareSettings(Settings *settings)
{
    settings->enableMultiTouch(false);
}

// a free function which sets gBackgroundColor to blue
void setBackgroundToBlue()
{
	gBackgroundColor = Color( 0.4f, 0.4f, 0.9f );
}

void CustomCallbackApp::setup()
{
#if defined( CINDER_GLES2 )
    mContext = gl::setGlesContext();
    gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
#endif

	setBackgroundToBlue();

	mCircle.mPos = Vec2f( 50, 50 );
	mCircle.mRadius = 50;
}

void CustomCallbackApp::mouseDown( MouseEvent event )
{
	timeline().apply( &mCircle.mPos, Vec2f( event.getPos() ), 2.0f, EaseInOutCubic() )
			.startFn( ColorToGreenFunctor( &gBackgroundColor ) )
			.updateFn( std::bind( &Circle::posUpdate, &mCircle ) )
			.finishFn( setBackgroundToBlue );
}

void CustomCallbackApp::draw()
{
#if defined( CINDER_GLES2 )
    mContext->bind();
#endif
    
	gl::clear( gBackgroundColor ); 	
	mCircle.draw();

#if defined( CINDER_GLES2 )
    mContext->unbind();
#endif    
}


CINDER_APP_NATIVE( CustomCallbackApp, RendererGl )
