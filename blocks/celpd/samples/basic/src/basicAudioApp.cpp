#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/Text.h"
#include "cinder/Utilities.h"

#include "celpd.h"

#include "propipe/Draw.h"
#include "propipe/TextureFont.h"
#include "propipe/Matrices.h"

using namespace cel;
using namespace ci;
using namespace ci::app;
using namespace std;

class BasicAudioApp : public AppNative {
  public:
	void prepareSettings( Settings *settings ) { settings->enableMultiTouch( false ); }
	void setup();
	void mouseDown( MouseEvent event );
	void keyDown( KeyEvent event );
	void draw();
	void resize( ResizeEvent event );
#if defined( CINDER_ANDROID )
	void resume( bool renewContext );
    void pause();
    void destroy();
#endif

	void setupMatrices();

	pp::Matrices mMatrices;

	Font                   mFont;
	pp::TextureFontRef     mTextureFont;

	pp::RendererRef        mRenderer;
	pp::DrawRef            mDraw;
	pp::TextureFontDrawRef mFontDraw;

    CelPdRef               mAudio;
};

void BasicAudioApp::setup()
{
    CI_LOGD("OSL: ACTIVITY SETUP");

    mAudio = CelPd::init(0, 2, 44100);
    mAudio->openFile("hello.pd", "/mnt/sdcard/pd");
    mAudio->play();

	mFont = Font( loadFile("/system/fonts/DroidSerif-Italic.ttf"), 40 );

	mTextureFont = pp::TextureFont::create( mFont );

	mRenderer = pp::Renderer::create();
	mFontDraw = pp::TextureFontDraw::create( mRenderer );
	mDraw = pp::Draw::create( mRenderer );

	setupMatrices();
}

void BasicAudioApp::resize( ResizeEvent event )
{
	//  Resize or orientation change
	setupMatrices();
}

#if defined( CINDER_ANDROID )

void BasicAudioApp::resume(bool renewContext)
{
    CI_LOGD("OSL: ACTIVITY RESUME");

	if (renewContext) {
		//  Release GL resources
		mTextureFont.reset();
		mDraw.reset();
		mFontDraw.reset();
		mRenderer.reset();

		//  Recreate GL resources
		mTextureFont = pp::TextureFont::create( mFont );
		mRenderer = pp::Renderer::create();
		mDraw = pp::Draw::create(mRenderer);
		mFontDraw = pp::TextureFontDraw::create( mRenderer );
	}

    mAudio->play();
}

void BasicAudioApp::pause()
{
    CI_LOGD("OSL: ACTIVITY PAUSE");
    mAudio->pause();
}

void BasicAudioApp::destroy()
{
    CI_LOGD("OSL: ACTIVITY DESTROY");
    mAudio->pause();
    mAudio->close();
    mAudio.reset();
}

#endif

void BasicAudioApp::keyDown( KeyEvent event )
{
}

void BasicAudioApp::mouseDown( MouseEvent event )
{
	console() << mFont.getName() << std::endl;
}

void BasicAudioApp::setupMatrices()
{
	Vec2i windowSize = getWindowSize();
	mMatrices.setMatricesWindow(windowSize.x, windowSize.y);
}

void BasicAudioApp::draw()
{
	gl::enableAlphaBlending();
	gl::clear( Color( 0, 0, 0 ) );

	std::string str( "Basic Audio" );
	Rectf boundsRect( 40, mTextureFont->getAscent() + 40, getWindowWidth() - 40, getWindowHeight() - 40 );

	mRenderer->bindProg();
	mRenderer->setModelView( mMatrices.getModelView() );
	mRenderer->setProjection( mMatrices.getProjection() );

	mFontDraw->setColor( ColorA( 0.17f, 0.72f, 0.88f, 1.0f ) );
	mFontDraw->drawStringWrapped( *mTextureFont, str, boundsRect );

	// Draw FPS
	mFontDraw->setColor( Color::white() );
	mFontDraw->drawString( *mTextureFont, toString( floor(getAverageFps()) ) + " FPS", Vec2f( 10, getWindowHeight() - mTextureFont->getDescent() ) );

	mRenderer->unbindProg();
}

CINDER_APP_NATIVE( BasicAudioApp, RendererGl(0) )

