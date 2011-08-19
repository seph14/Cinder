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

using namespace std;

class AndroidTestES2 : public AppNative {
public:
    gl::GlesContext* context;

	void setup();
	void draw();
	
	gl::Texture	mTexture;	
	Font mFont;
};

void AndroidTestES2::setup()
{
    context = new gl::GlesContext();

	try {
		console() << "Loading logo image" << endl;
		mTexture = gl::Texture( loadImage( loadResource("cinder_logo.png") ) );
		console() << "success!!!" << endl;
	}
	catch( ... ) {
		console() << "unable to load the texture file!" << std::endl;
	}

	string fontFile("/system/fonts/DroidSans.ttf");
	DataSourceRef fontData = loadFile(fontFile);
	mFont = Font(fontData, 16);
	console() << "Loaded font name " << mFont.getName() << " num glyphs " << mFont.getNumGlyphs() << endl;

    context->setMatricesWindow(getWindowWidth(), getWindowHeight());
}

void AndroidTestES2::draw()
{
    context->bind();

	gl::clear( Color( 0.2f, 0.2f, 0.2f ) );

    context->attr().drawLine(Vec2f(0, 0), Vec2f(200, 200));

    context->unbind();
}

CINDER_APP_NATIVE( AndroidTestES2, RendererGl )

