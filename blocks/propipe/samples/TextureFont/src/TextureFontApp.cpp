#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/Text.h"
#include "cinder/Utilities.h"

#include "propipe/Draw.h"
#include "propipe/TextureFont.h"
#include "propipe/Matrices.h"

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

	void setupView();

	pp::Matrices mMatrices;

	FontRef                mFont;
	pp::TextureFontRef     mTextureFont;

	pp::RendererRef        mRenderer;
	pp::DrawRef            mDraw;
	pp::TextureFontDrawRef mFontDraw;

    TextBox                mTextBox;
    Rectf                  mTextBounds;
};

void TextureFontApp::setup()
{
#if defined( CINDER_COCOA_TOUCH )
	mFont = Font::create( "Cochin-Italic", 24 );
#elif defined( CINDER_COCOA )
	mFont = Font::create( "BigCaslon-Medium", 24 );
#elif defined( CINDER_ANDROID )
	mFont = Font::getDefault();
#else
	mFont = Font::create( "Times New Roman", 24 );
#endif

	mTextureFont = pp::TextureFont::create( mFont );

	mRenderer = pp::Renderer::create();
	mFontDraw = pp::TextureFontDraw::create( mRenderer );
	mDraw = pp::Draw::create( mRenderer );

	setupView();
}

void TextureFontApp::resize( ResizeEvent event )
{
	//  Resize or orientation change
	setupView();
}

#if defined( CINDER_ANDROID )

void TextureFontApp::resume(bool renewContext)
{
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
        std::string str( "Granted, then, that certain transformations do happen, it is essential that we should regard them in the philosophic manner of fairy tales, not in the unphilosophic manner of science and the \"Laws of Nature.\" When we are asked why eggs turn into birds or fruits fall in autumn, we must answer exactly as the fairy godmother would answer if Cinderella asked her why mice turned into horses or her clothes fell from her at twelve o'clock. We must answer that it is MAGIC. It is not a \"law,\" for we do not understand its general formula." );
	}
}
#endif

void TextureFontApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case '=':
		case '+':
			mFont = Font::create( mFont->getName(), mFont->getSize() + 1 );
			mTextureFont = pp::TextureFont::create( mFont );
		break;
		case '-':
			mFont = Font::create( mFont->getName(), mFont->getSize() - 1 );
			mTextureFont = pp::TextureFont::create( mFont );
		break;
	}
}

void TextureFontApp::mouseDown( MouseEvent event )
{
#if ! defined( CINDER_ANDROID )
	mFont = Font::create( Font::getNames()[Rand::randInt() % Font::getNames().size()], mFont.getSize() );
	mTextureFont = pp::TextureFont::create( mFont );
#endif
}

void TextureFontApp::setupView()
{
	Vec2i windowSize = getWindowSize();
	mMatrices.setMatricesWindow(windowSize.x, windowSize.y);

	std::string str( "Granted, then, that certain transformations do happen, it is essential that we should regard them in the philosophic manner of fairy tales, not in the unphilosophic manner of science and the \"Laws of Nature.\" When we are asked why eggs turn into birds or fruits fall in autumn, we must answer exactly as the fairy godmother would answer if Cinderella asked her why mice turned into horses or her clothes fell from her at twelve o'clock. We must answer that it is MAGIC. It is not a \"law,\" for we do not understand its general formula." );
	mTextBounds = Rectf( 40, mTextureFont->getAscent() + 40, getWindowWidth() - 40, getWindowHeight() - 40 );
    mTextBox = TextBox().font( mFont ).text( str ).size( mTextBounds.getWidth(), mTextBounds.getHeight() );
}

void TextureFontApp::draw()
{
	gl::enableAlphaBlending();
	gl::clear( Color( 0, 0, 0 ) );

	mRenderer->bindProg();
	mRenderer->setModelView( mMatrices.getModelView() );
	mRenderer->setProjection( mMatrices.getProjection() );

	mRenderer->setColor( ColorA( 0.17f, 0.72f, 0.88f, 1.0f ) );
    mTextureFont->drawGlyphs(*mRenderer, mTextBox.measureGlyphs(), mTextBounds.getUpperLeft());
	// mFontDraw->drawStringWrapped( *mTextureFont, str, boundsRect );

	// Draw FPS
	mRenderer->setColor( Color::white() );
	mFontDraw->drawString( *mTextureFont, toString( floor(getAverageFps()) ) + " FPS", Vec2f( 10, getWindowHeight() - mTextureFont->getDescent() ) );

	// Draw Font Name
	float fontNameWidth = mTextureFont->measureString( mTextureFont->getName() ).x;
	mFontDraw->drawString( *mTextureFont, mTextureFont->getName(), Vec2f( getWindowWidth() - fontNameWidth - 10, getWindowHeight() - mTextureFont->getDescent() ) );

	//  mDraw shares its renderer with mFontDraw, so not required to bind prog and set up matrices again
	mRenderer->setColor( ColorA::white() );
	mDraw->drawStrokedRect( mTextBounds );

	mRenderer->unbindProg();
}

CINDER_APP_NATIVE( TextureFontApp, RendererGl(0) )

