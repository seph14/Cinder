#include "cinder/app/AppNative.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    CI_LOGI("GL %s = %s\n", name, v);
}

class ImageFileTestApp : public AppNative {
 public: 	
	void setup();
	void keyDown( KeyEvent event );
	
	void update();
	void draw();
	
	gl::Texture		mTexture;	
	gl::GlslProg	mShader;
	float			mAngle;
};


void ImageFileTestApp::setup()
{
   printGLString("Version", GL_VERSION);
   printGLString("Vendor", GL_VENDOR);
   printGLString("Renderer", GL_RENDERER);
   printGLString("Extensions", GL_EXTENSIONS);

	try {
		mTexture = gl::Texture( loadImage( loadResource( RES_IMAGE_JPG ) ) );
	}
	catch( ... ) {
		console() << "unable to load the texture file!" << std::endl;
	}
	
 	try {
 		mShader = gl::GlslProg( loadResource( RES_PASSTHRU_VERT ), loadResource( RES_BLUR_FRAG ) );
 	}
 	catch( gl::GlslProgCompileExc &exc ) {
 		console() << "Shader compile error: " << std::endl;
 		console() << exc.what();
 	}
 	catch( ... ) {
 		console() << "Unable to load shader" << std::endl;
 	}
	
	mAngle = 0.0f;
}

void ImageFileTestApp::keyDown( KeyEvent event )
{
	if( event.getCode() == app::KeyEvent::KEY_f ) {
		setFullScreen( ! isFullScreen() );
	}
}

void ImageFileTestApp::update()
{
	mAngle += 0.05f;
}

void ImageFileTestApp::draw()
{
	gl::clear();

	mTexture.enableAndBind();
	// mShader.bind();
	// mShader.uniform( "tex0", 0 );
	// mShader.uniform( "sampleOffset", Vec2f( cos( mAngle ), sin( mAngle ) ) * ( 3.0f / getWindowWidth() ) );
	// CI_LOGI("XXX gl::drawSolidRect");
	// gl::drawSolidRect( getWindowBounds() );

	mTexture.unbind();
}


CINDER_APP_NATIVE( ImageFileTestApp, RendererGl )
