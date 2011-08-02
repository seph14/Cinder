#include "cinder/app/AppImplAndroidRendererGl.h"
#include "cinder/app/App.h"
#include "cinder/app/Renderer.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"

#include <android/native_window.h>

namespace cinder { namespace app {

bool sMultisampleSupported = false;
int  sArbMultisampleFormat;

AppImplAndroidRendererGl::AppImplAndroidRendererGl( App *aApp, RendererGl *aRenderer )
	: mApp(aApp), mRenderer( aRenderer )
{
}

void AppImplAndroidRendererGl::initialize( ANativeWindow* window, int32_t& width, int32_t& height )
{
    //  Create GL context and surface
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 5,
            EGL_GREEN_SIZE, 6,
            EGL_RED_SIZE, 5,
            EGL_NONE
    };

    EGLint w, h, dummy, format;
    EGLint     numConfigs;
    EGLConfig  config;

    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(mDisplay, 0, 0);

    eglChooseConfig(mDisplay, attribs, &config, 1, &numConfigs);

    eglGetConfigAttrib(mDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);

    mSurface = eglCreateWindowSurface(mDisplay, config, window, NULL);
    mContext = eglCreateContext(mDisplay, config, NULL, NULL);

    makeCurrentContext();

    //  Query and save surface dimensions
    eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &w);
    eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &h);

    width  = w;
    height = h;

    // Initialize GL state
    glDisable(GL_DITHER);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

#if ! defined( CINDER_GLES2 )
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
#endif
}

void AppImplAndroidRendererGl::makeCurrentContext()
{
    if (eglMakeCurrent(mDisplay, mSurface, mSurface, mContext) == EGL_FALSE) {
        CI_LOGW("makeCurrentContext(): Unable to eglMakeCurrent");
    }
}

void AppImplAndroidRendererGl::swapBuffers()
{
    eglSwapBuffers(mDisplay, mSurface);
}

void AppImplAndroidRendererGl::defaultResize()
{
	glViewport( 0, 0, mApp->getWindowWidth(), mApp->getWindowHeight() );
	cinder::CameraPersp cam( mApp->getWindowWidth(), mApp->getWindowHeight(), 60.0f );

#if ! defined( CINDER_GLES2 )
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( cam.getProjectionMatrix().m );

	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( cam.getModelViewMatrix().m );
	glScalef( 1.0f, -1.0f, 1.0f );           // invert Y axis so increasing Y goes down.
	glTranslatef( 0.0f, (float)-mApp->getWindowHeight(), 0.0f );       // shift origin up to upper-left corner.
#endif
}

void AppImplAndroidRendererGl::teardown()
{
    if (mDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (mContext != EGL_NO_CONTEXT) {
            eglDestroyContext(mDisplay, mContext);
        }
        if (mSurface != EGL_NO_SURFACE) {
            eglDestroySurface(mDisplay, mSurface);
        }
        eglTerminate(mDisplay);
    }

    mDisplay = EGL_NO_DISPLAY;
    mContext = EGL_NO_CONTEXT;
    mSurface = EGL_NO_SURFACE;
}

bool AppImplAndroidRendererGl::isValidDisplay()
{
    return (mDisplay != EGL_NO_DISPLAY);
}

} }

