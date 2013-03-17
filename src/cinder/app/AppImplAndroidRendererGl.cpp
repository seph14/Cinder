#include "cinder/app/AppImplAndroidRendererGl.h"
#include "cinder/app/App.h"
#include "cinder/app/Renderer.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"

#include <android_native_app_glue.h>

namespace cinder { namespace app {

const char* EGLErrorString()
{
   EGLint nErr = eglGetError();
   switch(nErr){
      case EGL_SUCCESS:
         return "EGL_SUCCESS";
      case EGL_BAD_DISPLAY:
         return "EGL_BAD_DISPLAY";
      case EGL_NOT_INITIALIZED:
         return "EGL_NOT_INITIALIZED";
      case EGL_BAD_ACCESS:
         return "EGL_BAD_ACCESS";
      case EGL_BAD_ALLOC:
         return "EGL_BAD_ALLOC";
      case EGL_BAD_ATTRIBUTE:
         return "EGL_BAD_ATTRIBUTE";
      case EGL_BAD_CONFIG:
         return "EGL_BAD_CONFIG";
      case EGL_BAD_CONTEXT:
         return "EGL_BAD_CONTEXT";
      case EGL_BAD_CURRENT_SURFACE:
         return "EGL_BAD_CURRENT_SURFACE";
      case EGL_BAD_MATCH:
         return "EGL_BAD_MATCH";
      case EGL_BAD_NATIVE_PIXMAP:
         return "EGL_BAD_NATIVE_PIXMAP";
      case EGL_BAD_NATIVE_WINDOW:
         return "EGL_BAD_NATIVE_WINDOW";
      case EGL_BAD_PARAMETER:
         return "EGL_BAD_PARAMETER";
      case EGL_BAD_SURFACE:
         return "EGL_BAD_SURFACE";
      default:
         return "unknown";
   }
}

AppImplAndroidRendererGl::AppImplAndroidRendererGl( App *aApp, struct android_app *androidApp )
	: mApp(aApp), mAndroidApp(androidApp)
{
}

void AppImplAndroidRendererGl::initialize( int32_t* width, int32_t* height )
{
    CI_LOGD("AppImplAndroidRendererGl::initialize() 1");
    //  Create GL context and surface
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 5,
            EGL_GREEN_SIZE, 6,
            EGL_RED_SIZE, 5,
            EGL_DEPTH_SIZE, 16,
#if defined( CINDER_GLES2 )
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
#endif
            EGL_NONE
    };

    EGLint w, h, dummy, format;
    EGLint     numConfigs;
    EGLConfig  config;

    CI_LOGD("AppImplAndroidRendererGl::initialize() 2");
    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(mDisplay, 0, 0);

    CI_LOGD("AppImplAndroidRendererGl::initialize() 3");
    eglChooseConfig(mDisplay, attribs, &config, 1, &numConfigs);

    eglGetConfigAttrib(mDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(mAndroidApp->window, 0, 0, format);

    CI_LOGD("AppImplAndroidRendererGl::initialize() 4");
    mSurface = eglCreateWindowSurface(mDisplay, config, mAndroidApp->window, NULL);
    if (mSurface == EGL_NO_SURFACE) {
        CI_LOGE("Error in eglCreateWindowSurface, %s", EGLErrorString());
    }

#if defined( CINDER_GLES2 )
    EGLint eglAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    mContext = eglCreateContext(mDisplay, config, EGL_NO_CONTEXT, eglAttribs);
#else
    mContext = eglCreateContext(mDisplay, config, NULL, NULL);
#endif

    if (mContext == EGL_NO_CONTEXT) {
        CI_LOGE("eglCreateContext returned EGL_NO_CONTEXT");
    }
    else {
        CI_LOGD("eglCreateContext succeeded, returned %p", mContext);
    }

    CI_LOGD("AppImplAndroidRendererGl::initialize() 5");
    makeCurrentContext();

    //  Query and save surface dimensions
    eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &w);
    eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &h);

    *width = w; *height = h;

    CI_LOGD("AppImplAndroidRendererGl::initialize() 6");
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
        CI_LOGE("makeCurrentContext(): Unable to eglMakeCurrent");
    }
}

void AppImplAndroidRendererGl::swapBuffers()
{
    if (eglSwapBuffers(mDisplay, mSurface) == EGL_FALSE) {
        CI_LOGE("eglSwapBuffers returned EGL_FALSE : %s", EGLErrorString());
    }
}

void AppImplAndroidRendererGl::defaultResize()
{
    int width  = mApp->getWindowWidth();
    int height = mApp->getWindowHeight();
    CI_LOGD("Setting viewport to %d x %d", width, height); 
    glViewport( 0, 0, width, height );

#if ! defined( CINDER_GLES2 )
    gl::setMatricesWindow(width, height);
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
    return (mDisplay != EGL_NO_DISPLAY && mAndroidApp->window != NULL);
}

} }

