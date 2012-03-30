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

#if defined( CINDER_ANDROID )
  #define LIBPD_DLL "/data/data/com.expandingbrain.celaudio/lib/libpdnative.so"
#else
  #define LIBPD_DLL "libpd.dll"
#endif

using namespace cel;
using namespace cel::pd;
using namespace ci;
using namespace ci::app;
using namespace std;

class BasicAudioApp : public AppNative, public pd::Receiver {
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

    virtual void onFloat(const std::string& dest, float value);

    void setupMatrices();

    pp::Matrices mMatrices;

    Font                   mFont;
    pp::TextureFontRef     mTextureFont;

    pp::RendererRef        mRenderer;
    pp::DrawRef            mDraw;
    pp::TextureFontDrawRef mFontDraw;

    PdRef               mPd;
};

void BasicAudioApp::setup()
{
    CI_LOGD("OSL: ACTIVITY SETUP");

    mPd = Pd::create(true, LIBPD_DLL);
    if (!mPd) {
        CI_LOGE("Error initializing PD");
    }

    mPd->init(0, 2, 44100);

    fs::path internalData = getInternalDataPath(); 
    copyResource("oggread~.pd_linux", internalData, true);
    copyResource("oggtest.pd", internalData, true);

    mPd->addToSearchPath(internalData);
    mPd->subscribe(*this) << "timer";
    mPd->openFile("oggtest.pd", internalData);

    mPd->play();

    //  Start the ogg file playing
    CI_LOGD("Sending start bangs");
    mPd->sendBang("open");
    mPd->sendBang("start");

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
        mRenderer    = pp::Renderer::create();
        mDraw        = pp::Draw::create(mRenderer);
        mFontDraw    = pp::TextureFontDraw::create( mRenderer );
    }

    mPd->play();
}

void BasicAudioApp::pause()
{
    CI_LOGD("OSL: ACTIVITY PAUSE");
    mPd->pause();
}

void BasicAudioApp::destroy()
{
    CI_LOGD("OSL: ACTIVITY DESTROY");
    mPd->pause();
    mPd->close();
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
    mFontDraw->drawString( *mTextureFont, toString( floor(getAverageFps()) ) + " FPS", 
            Vec2f( 10, getWindowHeight() - mTextureFont->getDescent() ) );

    mRenderer->unbindProg();
}

void BasicAudioApp::onFloat(const std::string& dest, float value)
{
    CI_LOGD("BasicAudioApp::onFloat %s %f", dest.c_str(), value);
}

CINDER_APP_NATIVE( BasicAudioApp, RendererGl(0) )

