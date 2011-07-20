#include "cinder/app/AppNative.h"
#include "cinder/Rand.h"

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
}

void AndroidTest::draw()
{
	gl::setMatricesWindow( getWindowSize() );
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
}

CINDER_APP_NATIVE( AndroidTest, RendererGl )

