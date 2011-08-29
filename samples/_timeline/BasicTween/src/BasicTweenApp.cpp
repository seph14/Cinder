#include "cinder/app/AppNative.h"

#if defined( CINDER_GLES2 )
#include "cinder/gl/gles2.h"
#endif

using namespace std;
using namespace ci;
using namespace ci::app;

class BasicTweenApp : public AppNative {
#if defined( CINDER_GLES2 )
    gl::GlesContextRef mContext;
#endif
  public:
    void prepareSettings(Settings *settings);
	void setup();
	void mouseDown( MouseEvent event );
	void draw();
  
	Vec2f mBlackPos, mWhitePos;
};

void BasicTweenApp::prepareSettings(Settings *settings)
{	
    settings->enableMultiTouch(false);
}

void BasicTweenApp::setup()
{	
#if defined( CINDER_GLES2 )
    mContext = gl::setGlesContext();
    gl::setMatricesWindow(getWindowWidth(), getWindowHeight());
#endif
	mBlackPos = mWhitePos = getWindowCenter();
}

void BasicTweenApp::mouseDown( MouseEvent event )
{
	// the call to apply() replaces any existing tweens on mBlackPos with this new one
	timeline().apply( &mBlackPos, (Vec2f)event.getPos(), 2.0f, EaseInCubic() );
	// the call to appendTarget() ensures that any other tweens on mWhitePos will complete before this new one begins
	timeline().appendTarget( &mWhitePos, (Vec2f)event.getPos(), 3.0f, EaseOutQuint() );
}

void BasicTweenApp::draw()
{
#if defined( CINDER_GLES2 )
    mContext->bind();
#endif

	gl::clear( Color( 0.5f, 0.5f, 0.5f ) );
	
	gl::color( Color::black() );
	gl::drawSolidCircle( mBlackPos, 20.0f );
	
	gl::color( Color::white() );
	gl::drawSolidCircle( mWhitePos, 16.0f );

#if defined( CINDER_GLES2 )
    mContext->unbind();
#endif    
}

// This line tells Cinder to actually create the application
CINDER_APP_NATIVE( BasicTweenApp, RendererGl )
