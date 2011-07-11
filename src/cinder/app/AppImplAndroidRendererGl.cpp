#include "cinder/app/AppImplAndroidRendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"
#include "cinder/Camera.h"

namespace cinder { namespace app {

bool sMultisampleSupported = false;
int sArbMultisampleFormat;

AppImplAndroidRendererGl::AppImplAndroidRendererGl( App *aApp, RendererGl *aRenderer )
	: mRenderer( aRenderer )
{
}

} }

