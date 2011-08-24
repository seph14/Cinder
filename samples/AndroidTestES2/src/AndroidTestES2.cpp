#include "cinder/app/AppNative.h"
#include "cinder/Rand.h"

#include "cinder/ImageIo.h"
#include "cinder/DataSource.h"
#include "cinder/Stream.h"

#include "cinder/gl/Texture.h"
#include "cinder/gl/gles2.h"

#include "cinder/Font.h"
#include "cinder/Camera.h"

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
    CameraPersp    mCam;
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

    float aspectRatio = float(getWindowWidth()) / float(getWindowHeight());
    mCam.setPerspective(50.0f, aspectRatio, 0.1f, 1000.0f);
    mCam.setEyePoint(Vec3f(0, 0, 10.0f));
    mCam.lookAt(Vec3f(0, 0, 0));

    glEnable(GL_CULL_FACE);
}

void AndroidTestES2::draw()
{
    mContext->bind();

    setMatricesWindow(getWindowWidth(), getWindowHeight());
	clear( Color( 0.2f, 0.2f, 0.2f ) );

    color(ColorA(1.0f, 1.0f, 0, 1.0f));
    drawLine(Vec2f(0, 0), Vec2f(200, 200));

    color(ColorA(1.0f, 0, 0, 1.0f));

    if (mTexture) {
        Vec2f lr(getWindowWidth(), getWindowHeight());
        gl::draw(mTexture, Rectf(lr.x-100.0f, lr.y-100.0f, lr.x, lr.y));
    }

    color(Color::white());
    setMatrices(mCam);
    rotate(Vec3f(60.0f, 60.0f, 60.0f));
    drawCube( Vec3f(0, 0, 0), Vec3f(3.0f, 3.0f, 3.0f) );
    // drawStrokedCube( Vec3f(0, 0, 0), Vec3f(3.0f, 3.0f, 3.0f) );

    mContext->unbind();
}

CINDER_APP_NATIVE( AndroidTestES2, RendererGl )

