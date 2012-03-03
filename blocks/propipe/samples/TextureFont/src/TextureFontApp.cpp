#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"
#include "cinder/Text.h"
#include "cinder/Utilities.h"

#include "propipe/TextureFont.h"
#include "propipe/Draw.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TextureFontApp : public AppNative {
  public:
	void prepareSettings( Settings *settings ) { settings->enableMultiTouch( false ); }
	void setup();
	void mouseDown( MouseEvent event );
	void keyDown( KeyEvent event );
	void draw();
	void resize( ResizeEvent event );
#if defined( CINDER_ANDROID )
    void resume( bool renewContext );
#endif

	void setupMVP();

    Matrix44f                  mMVP;
	Font			           mFont;
	pp::TextureFontRef         mTextureFont;
	pp::TextureFontRendererRef mTextRender;
	pp::DrawRendererRef        mDraw;
};

void TextureFontApp::setup()
{
#if defined( CINDER_COCOA_TOUCH )
	mFont = Font( "Cochin-Italic", 24 );
#elif defined( CINDER_COCOA )
	mFont = Font( "BigCaslon-Medium", 24 );
#elif defined( CINDER_ANDROID )
    // mFont = Font( loadFile("/system/fonts/DroidSans.ttf"), 24 );
    mFont = Font( loadFile("/system/fonts/DroidSerif-Italic.ttf"), 24 );
#else
	mFont = Font( "Times New Roman", 24 );
#endif

	mTextureFont = pp::TextureFont::create( mFont );
	mTextRender  = pp::TextureFont::createRenderer();
	mDraw        = pp::DrawRenderer::create();
	setupMVP();
}

void TextureFontApp::resize( ResizeEvent event )
{
	//  Resize or orientation change
	setupMVP();
}

#if defined( CINDER_ANDROID )

void TextureFontApp::resume(bool renewContext)
{
    if (renewContext) {
        //  Release GL resources
        mTextureFont.reset();
		mTextRender.reset();
		mDraw.reset();

        //  Recreate GL resources
        mTextureFont = pp::TextureFont::create( mFont );
		mTextRender  = pp::TextureFont::createRenderer();
		mDraw        = pp::DrawRenderer::create();
    }
}
#endif

void TextureFontApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case '=':
		case '+':
			mFont = Font( mFont.getName(), mFont.getSize() + 1 );
			mTextureFont = pp::TextureFont::create( mFont );
		break;
		case '-':
			mFont = Font( mFont.getName(), mFont.getSize() - 1 );
			mTextureFont = pp::TextureFont::create( mFont );
		break;
	}
}

void TextureFontApp::mouseDown( MouseEvent event )
{
#if ! defined( CINDER_ANDROID )
	mFont = Font( Font::getNames()[Rand::randInt() % Font::getNames().size()], mFont.getSize() );
#endif
	console() << mFont.getName() << std::endl;
	mTextureFont = pp::TextureFont::create( mFont );
}

void TextureFontApp::setupMVP()
{
	//  Set up an ortho modelview-projection matrix
    CameraOrtho cam;
    Vec2i windowSize = getWindowSize();
    cam.setOrtho(0, windowSize.x, windowSize.y, 0, 1.0f, -1.0f);
    mMVP = cam.getProjectionMatrix() * cam.getModelViewMatrix();
}

void TextureFontApp::draw()
{
    // pp::TextureFont::IRenderer& textRender = pp::TextureFont::renderer();
	gl::enableAlphaBlending();
	gl::clear( Color( 0, 0, 0 ) );

	std::string str( "Granted, then, that certain transformations do happen, it is essential that we should regard them in the philosophic manner of fairy tales, not in the unphilosophic manner of science and the \"Laws of Nature.\" When we are asked why eggs turn into birds or fruits fall in autumn, we must answer exactly as the fairy godmother would answer if Cinderella asked her why mice turned into horses or her clothes fell from her at twelve o'clock. We must answer that it is MAGIC. It is not a \"law,\" for we do not understand its general formula." );
	Rectf boundsRect( 40, mTextureFont->getAscent() + 40, getWindowWidth() - 40, getWindowHeight() - 40 );

	mTextRender->bind();
    mTextRender->setMVP( mMVP );
    mTextRender->setColor( ColorA( 0.17f, 0.72f, 0.88f, 1.0f ) );

	mTextRender->drawStringWrapped( *mTextureFont, str, boundsRect );

	// Draw FPS
	mTextRender->setColor( Color::white() );
	mTextRender->drawString( *mTextureFont, toString( floor(getAverageFps()) ) + " FPS", Vec2f( 10, getWindowHeight() - mTextureFont->getDescent() ) );
    
    // Draw Font Name
	float fontNameWidth = mTextureFont->measureString( mTextureFont->getName() ).x;
	mTextRender->drawString( *mTextureFont, mTextureFont->getName(), Vec2f( getWindowWidth() - fontNameWidth - 10, getWindowHeight() - mTextureFont->getDescent() ) );

	mTextRender->unbind();

	mDraw->bind();
	mDraw->setColor(ColorA::white());
	mDraw->setModelView(Matrix44f::identity());
	mDraw->setProjection(mMVP);
	mDraw->drawStrokedRect(boundsRect);
	mDraw->unbind();
}

CINDER_APP_NATIVE( TextureFontApp, RendererGl(0) )
