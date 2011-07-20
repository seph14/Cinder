#include "cinder/app/AppNative.h"
#include "cinder/Rand.h"

#include "cinder/DataSource.h"
#include "cinder/Stream.h"

using namespace ci;
using namespace ci::app;

using namespace std;

class AndroidTest : public AppNative {
public:
	void setup();
	void draw();
};

void AndroidTest::setup()
{
	console() << "AndroidTest" << endl;
	DataSourceRef data = loadResource("hello.txt");
	console() << "Loaded hello.txt" << endl;
	IStreamRef stream = data->createStream();
	int size = stream->size();
	console() << "Stream size: " << size << endl;
	char* input = new char[size+1];
	int read = stream->readDataAvailable(input, 1024);
	input[read] = '\0';
	console() << "Read " << read << " bytes from stream" << endl;
	console() << "value: " << input << endl;

	// string text;
	// stream->readFixedString(&text, size);
	// console() << "Read from hello.txt: " << text << endl;
}

void AndroidTest::draw()
{
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
}

CINDER_APP_NATIVE( AndroidTest, RendererGl )

