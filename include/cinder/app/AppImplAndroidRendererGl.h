#pragma once

#include <EGL/egl.h>

extern "C" {
    struct ANativeWindow;
}

namespace cinder { namespace app {

class App;
class RendererGl;

class AppImplAndroidRendererGl 
{
 public:
	AppImplAndroidRendererGl( App *aApp, RendererGl *aRenderer );

    void initialize( ANativeWindow* window, int32_t& width, int32_t& height );
    void makeCurrentContext();
    void swapBuffers();
    void defaultResize();

    void teardown();
    bool isValidDisplay();

 protected:
	App         *mApp;
	RendererGl	*mRenderer;

    EGLSurface mSurface;
    EGLContext mContext;
    EGLDisplay mDisplay;
};

} } // namespace cinder::app
