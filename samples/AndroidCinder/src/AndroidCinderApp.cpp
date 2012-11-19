#include "cinder/app/AppNative.h"
#include "cinder/Camera.h"

#include <android_native_app_glue.h>
#include <jni.h>

using namespace ci;
using namespace ci::app;

class AndroidCinderApp : public AppNative {
  public:
    virtual void setup();
    virtual void resume(bool renewContext);
    virtual void destroy();
    virtual void draw();

    void helloCinder();

    Matrix44f   mModelView;
    CameraPersp mCam;
};

void AndroidCinderApp::setup()
{
    mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
    mCam.lookAt( Vec3f( 0, 0, 3 ), Vec3f::zero() );	
    glEnable(GL_CULL_FACE);

    //  Use JNI to call the Java method AndroidCinderActivity.helloCinder()
    initJNI();
    helloCinder();
}

void AndroidCinderApp::helloCinder()
{
    jclass androidCinderActivity = findClass("org/libcinder/android/AndroidCinderActivity");

    JNIEnv* env = getJNIEnv();
    jmethodID helloCinder = env->GetMethodID(androidCinderActivity, "helloCinder", "()V");
    env->CallVoidMethod(getActivity(), helloCinder);

    //  Free JNI reference
    env->DeleteLocalRef(androidCinderActivity);
}

void AndroidCinderApp::resume(bool renewContext)
{
    console() << "AndroidCinderApp resume(), renewContext " << renewContext << std::endl;
}

void AndroidCinderApp::destroy()
{
    console() << "AndroidCinderApp destroy()" << std::endl;
}

void AndroidCinderApp::draw()
{
    gl::clear( Color( 0.2f, 0.2f, 0.3f ) );
    gl::enableDepthRead();

    gl::setMatrices( mCam );
    gl::multModelView( mModelView );
    gl::drawColorCube( Vec3f::zero(), Vec3f( 1, 1, 1 ) );
}

CINDER_APP_NATIVE( AndroidCinderApp, RendererGl )
