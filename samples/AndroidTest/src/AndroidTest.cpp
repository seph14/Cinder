#include "cinder/app/AppNative.h"
#include "cinder/Rand.h"

#include "cinder/ImageIo.h"
#include "cinder/DataSource.h"
#include "cinder/Stream.h"

#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"

#include "cinder/Font.h"

using namespace ci;
using namespace ci::app;

using namespace std;

class AndroidTest : public AppNative {
public:
	void setup();
	void draw();
	
	gl::Texture		mTexture;	

	Font               mFont;
	gl::TextureFontRef mTexFont;
	gl::Texture        mFontTexture;
};

void AndroidTest::setup()
{
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

    mTexFont = gl::TextureFont::create( mFont );
    mFontTexture = mTexFont->getTexture();

	// XXX uncomment to trigger a crash in Font destructor
	// mFont = Font();

	// console() << "AndroidTest" << endl;
	// DataSourceRef data = loadResource("hello.txt");
	// console() << "Loaded hello.txt" << endl;
	// IStreamRef stream = data->createStream();
	// int size = stream->size();
	// console() << "Stream size: " << size << endl;
	// char* input = new char[size+1];
	// int read = stream->readDataAvailable(input, 1024);
	// input[read] = '\0';
	// console() << "Read " << read << " bytes from stream" << endl;
	// console() << "value: " << input << endl;

	// string text;
	// stream->readFixedString(&text, size);
	// console() << "Read from hello.txt: " << text << endl;
}

void AndroidTest::draw()
{
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( 0.2f, 0.2f, 0.2f ) );

	// if( mTexture )
	// 	gl::draw( mTexture, Vec2f( 0, 0 ) );
	if( mFontTexture )
		gl::draw( mFontTexture, Vec2f( 0, 0 ) );
}

CINDER_APP_NATIVE( AndroidTest, RendererGl )

