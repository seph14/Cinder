#include "cinder/app/AppNative.h"
#include "cinder/Rand.h"

#include "cinder/ImageIo.h"
#include "cinder/DataSource.h"
#include "cinder/Stream.h"

#include "cinder/gl/Texture.h"
#include "cinder/gl/gles2.h"

#include "cinder/Font.h"

using namespace ci;
using namespace ci::app;
using namespace ci::gl;

using namespace std;

class AndroidTestES2 : public AppNative {
public:
	void setup();
	void draw();
	
    GlesContextRef mContext;
	Texture	       mTexture;	
	Font mFont;
};

void AndroidTestES2::setup()
{
    mContext = setGlesContext();

	try {
		console() << "Loading logo image" << endl;
		mTexture = Texture( loadImage( loadResource("cinder_logo.png") ) );
		console() << "success!!!" << endl;
	}
	catch( ... ) {
		console() << "unable to load the texture file!" << std::endl;
	}

	string fontFile("/system/fonts/DroidSans.ttf");
	DataSourceRef fontData = loadFile(fontFile);
	mFont = Font(fontData, 16);
	console() << "Loaded font name " << mFont.getName() << " num glyphs " << mFont.getNumGlyphs() << endl;

    setMatricesWindow(getWindowWidth(), getWindowHeight());
    glDisable(GL_CULL_FACE);
}

void AndroidTestES2::draw()
{
    mContext->bind();

	clear( Color( 0.2f, 0.2f, 0.2f ) );

    color(ColorA(1.0f, 1.0f, 0, 1.0f));
    drawLine(Vec2f(0, 0), Vec2f(200, 200));

    color(ColorA(1.0f, 0, 0, 1.0f));
    drawSolidCircle( Vec2f(getWindowWidth() / 2.0f, getWindowHeight() / 2.0f), 
                                     40.0f, 32);

    if (mTexture) {
        color(ColorA(1.0f, 1.0f, 1.0f, 1.0f));
        Vec2f lr(getWindowWidth(), getWindowHeight());
        gl::draw(mTexture, Rectf(lr.x-100.0f, lr.y-100.0f, lr.x, lr.y));
    }

    mContext->unbind();
}

CINDER_APP_NATIVE( AndroidTestES2, RendererGl )

