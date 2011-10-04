#pragma once

#include <EGL/egl.h>

extern "C" {
    struct ANativeWindow;
    struct android_app;
}

namespace cinder { namespace app {

class App;
class RendererGl;

class AppImplAndroidRendererGl 
{
 public:
	AppImplAndroidRendererGl( App *aApp, struct android_app *androidApp );

    void initialize( int32_t& width, int32_t& height );
    void makeCurrentContext();
    void swapBuffers();
    void defaultResize();

    void teardown();
    bool isValidDisplay();

 protected:
	App         *mApp;
    android_app *mAndroidApp;

    EGLSurface mSurface;
    EGLContext mContext;
    EGLDisplay mDisplay;
};

} } // namespace cinder::app
