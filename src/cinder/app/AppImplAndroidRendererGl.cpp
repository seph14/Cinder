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
    CI_LOGW("XXX AppImplAndroidRendererGl::initialize");
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

#if defined( CINDER_GLES2 )
    EGLint eglAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    mContext = eglCreateContext(mDisplay, config, EGL_NO_CONTEXT, eglAttribs);
#else
    mContext = eglCreateContext(mDisplay, config, NULL, NULL);
#endif

    makeCurrentContext();

    //  Query and save surface dimensions
    eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &w);
    eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &h);

    width  = w;
    height = h;

    // Initialize GL state
    glDisable(GL_DITHER);
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
    int width = mApp->getWindowWidth();
    int height = mApp->getWindowHeight();
    CI_LOGW("Setting viewport to %d x %d", width, height); 
	glViewport( 0, 0, width, height );

#if ! defined( CINDER_GLES2 )
    gl::setMatricesWindow(width, height);
#endif
}

void AppImplAndroidRendererGl::teardown()
{
    if (mDisplay != EGL_NO_DISPLAY) {
        // XXX These teardown methods consistently hard-crash the HTC Evo 3D
        // running 2.3.3, not sure of a good workaround.

        // eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        // if (mContext != EGL_NO_CONTEXT) {
        //     eglDestroyContext(mDisplay, mContext);
        // }
        // CI_LOGW("DESTROY SURFACE");
        // if (mSurface != EGL_NO_SURFACE) {
        //     eglDestroySurface(mDisplay, mSurface);
        // }

        // eglTerminate(mDisplay);
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

