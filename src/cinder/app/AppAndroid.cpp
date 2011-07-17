#include "cinder/app/AppAndroid.h"

#include <time.h>

//  Android native activity sample code
#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

/**
 * Our saved state data.
 */
struct saved_state {
    float angle;
    int32_t x;
    int32_t y;
};

/**
 * Shared state for our app.
 */
struct engine {
    struct android_app* state;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

    int animating;

    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    int32_t width;
    int32_t height;

    struct saved_state savedState;

    struct timespec mStartTime;
    ci::app::AppAndroid* cinderApp;

    std::vector<ci::app::TouchEvent::Touch> touchesBegan;
    std::vector<ci::app::TouchEvent::Touch> touchesMoved;
    std::vector<ci::app::TouchEvent::Touch> touchesEnded;
};

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine* engine) {
    // initialize OpenGL ES and EGL

    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(engine->state->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->state->window, NULL);
    context = eglCreateContext(display, config, NULL, NULL);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        CI_LOGW("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;
    engine->savedState.angle = 0;

    // Initialize GL state.
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
    glOrthof( 0, w, h, 0, -1.0f, 1.0f );
    // glOrthof( 0, screenWidth, 0, screenHeight, -1.0f, 1.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glViewport( 0, 0, w, h );

    return 0;
}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) {
    if (engine->display == NULL) {
        // No display.
        return;
    }

    ci::app::AppAndroid& app = *(engine->cinderApp);
    app.privateUpdate__();
    app.privateDraw__();

    eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    static const char* actionNames[] = {
        "AMOTION_EVENT_ACTION_DOWN",
        "AMOTION_EVENT_ACTION_UP",
        "AMOTION_EVENT_ACTION_MOVE",
        "AMOTION_EVENT_ACTION_CANCEL",
        "AMOTION_EVENT_ACTION_OUTSIDE",
        "AMOTION_EVENT_ACTION_POINTER_DOWN",
        "AMOTION_EVENT_ACTION_POINTER_UP",
    };

    struct engine* engine = (struct engine*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        int32_t x = engine->savedState.x = AMotionEvent_getX(event, 0);
        int32_t y = engine->savedState.y = AMotionEvent_getY(event, 0);

        int32_t actionCode = AMotionEvent_getAction(event);
        int action = actionCode & AMOTION_EVENT_ACTION_MASK;
        int index  = (actionCode & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        const char* actionName = (action >= 0 && action <= 6) ? actionNames[action] : "UNKNOWN";
        CI_LOGI("Received touch action %s pointer index %d x %d y %d", actionName, index, 
                engine->savedState.x, engine->savedState.y);

        double timestamp = engine->cinderApp->getElapsedSeconds();
        if (action == AMOTION_EVENT_ACTION_DOWN) {
            engine->touchesBegan.push_back(ci::app::TouchEvent::Touch(ci::Vec2f(x, y),
                ci::Vec2f(x, y), index, timestamp, NULL));
        }
        else if (action == AMOTION_EVENT_ACTION_MOVE) {
            engine->touchesMoved.push_back(ci::app::TouchEvent::Touch(ci::Vec2f(x, y),
                ci::Vec2f(x, y), index, timestamp, NULL));
        }
        else if (action == AMOTION_EVENT_ACTION_UP) {
            engine->touchesEnded.push_back(ci::app::TouchEvent::Touch(ci::Vec2f(x, y),
                ci::Vec2f(x, y), index, timestamp, NULL));
        }

        return 1;
    }
    return 0;
}

static void engine_update_touches(struct engine* engine) 
{
    ci::app::AppAndroid& app = *(engine->cinderApp);
    if ( ! engine->touchesBegan.empty() ) {
        app.privateTouchesBegan__( ci::app::TouchEvent( engine->touchesBegan ) );
        engine->touchesBegan.clear();
    }
    if ( ! engine->touchesMoved.empty() ) {
        app.privateTouchesMoved__( ci::app::TouchEvent( engine->touchesMoved ) );
        engine->touchesMoved.clear();
    }
    if ( ! engine->touchesEnded.empty() ) {
        app.privateTouchesEnded__( ci::app::TouchEvent( engine->touchesEnded ) );
        engine->touchesEnded.clear();
    }
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            engine->state->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)engine->state->savedState) = engine->savedState;
            engine->state->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (engine->state->window != NULL) {
                engine_init_display(engine);

                ci::app::AppAndroid& app = *(engine->cinderApp);
                app.privateSetup__();
                app.privateResize__(ci::Vec2i(engine->width, engine->height));

                engine_draw_frame(engine);
                engine->animating = 1;
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, we start monitoring the accelerometer.
            if (engine->accelerometerSensor != NULL) {
                ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                        engine->accelerometerSensor);
                // We'd like to get 60 events per second (in us).
                ASensorEventQueue_setEventRate(engine->sensorEventQueue,
                        engine->accelerometerSensor, (1000L/60)*1000);
            }
            break;
        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            if (engine->accelerometerSensor != NULL) {
                ASensorEventQueue_disableSensor(engine->sensorEventQueue,
                        engine->accelerometerSensor);
            }
            // Also stop animating.
            engine->animating = 0;
            engine_draw_frame(engine);
            break;
    }
}

void android_run(struct engine* engine) 
{
    // Make sure glue isn't stripped.
    app_dummy();

    struct android_app* state = engine->state;
    state->userData = engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;

    // Prepare to monitor accelerometer
    engine->sensorManager = ASensorManager_getInstance();
    engine->accelerometerSensor = ASensorManager_getDefaultSensor(engine->sensorManager,
            ASENSOR_TYPE_ACCELEROMETER);
    engine->sensorEventQueue = ASensorManager_createEventQueue(engine->sensorManager,
            state->looper, LOOPER_ID_USER, NULL, NULL);

    if (state->savedState != NULL) {
        // We are starting with a previous saved state; restore from it.
        engine->savedState = *(struct saved_state*)state->savedState;
    }

    // loop waiting for stuff to do.

    while (1) {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(engine->animating ? 0 : -1, NULL, &events,
                (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // If a sensor has data, process it now.
            if (ident == LOOPER_ID_USER) {
                if (engine->accelerometerSensor != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(engine->sensorEventQueue,
                            &event, 1) > 0) {
                        // LOGI("accelerometer: x=%f y=%f z=%f",
                        //         event.acceleration.x, event.acceleration.y,
                        //         event.acceleration.z);
                    }
                }
            }

            // Update input
            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine_term_display(engine);
                return;
            }
        }

        //  Update engine touch state
        engine_update_touches(engine);

        if (engine->animating) {
            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.
            engine_draw_frame(engine);
        }
    }
}

namespace cinder { namespace app {

AppAndroid*				AppAndroid::sInstance = 0;
AppAndroid*				sInstance;

AppAndroid::AppAndroid()
	: App()
{
    mEngine = new engine;
    memset(mEngine, 0, sizeof(engine));
    mEngine->cinderApp = this;
	clock_gettime(CLOCK_MONOTONIC, &(mEngine->mStartTime));
	mLastAccel = mLastRawAccel = Vec3f::zero();
}

void AppAndroid::setAppState( struct android_app* state )
{
    mEngine->state = state;
}

void AppAndroid::launch( const char *title, int argc, char * const argv[] )
{
    cinder::app::App* app = cinder::app::AppAndroid::get();
    android_run(mEngine);
}

int	AppAndroid::getWindowWidth() const
{
    return mEngine->width;
}

int	AppAndroid::getWindowHeight() const
{
    return mEngine->height;
}

//! Enables the accelerometer
void AppAndroid::enableAccelerometer( float updateFrequency, float filterFactor )
{
	mAccelFilterFactor = filterFactor;
	
	if( updateFrequency <= 0 )
		updateFrequency = 30.0f;
	
	// [[UIAccelerometer sharedAccelerometer] setUpdateInterval:1.0 / updateFrequency];
	// CinderAppDelegateIPhone *appDel = (CinderAppDelegateIPhone *)[[UIApplication sharedApplication] delegate];
	// [[UIAccelerometer sharedAccelerometer] setDelegate:appDel];
}

void AppAndroid::disableAccelerometer() {
	
	// [[UIAccelerometer sharedAccelerometer] setDelegate:nil];
}

//! Returns the maximum frame-rate the App will attempt to maintain.
float AppAndroid::getFrameRate() const
{
	return 0;
}

//! Sets the maximum frame-rate the App will attempt to maintain.
void AppAndroid::setFrameRate( float aFrameRate )
{
}

//! Returns whether the App is in full-screen mode or not.
bool AppAndroid::isFullScreen() const
{
	return true;
}

//! Sets whether the active App is in full-screen mode based on \a fullScreen
void AppAndroid::setFullScreen( bool aFullScreen )
{
}

double AppAndroid::getElapsedSeconds() const
{
	struct timespec currentTime;
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	return ( (currentTime.tv_sec + currentTime.tv_nsec / 1e9) 
			- (mEngine->mStartTime.tv_sec + mEngine->mStartTime.tv_nsec / 1e9) );
}

std::string AppAndroid::getAppPath()
{ 
	// XXX TODO
	return std::string();
}

void AppAndroid::quit()
{
	return;
}

void AppAndroid::privatePrepareSettings__()
{
	prepareSettings( &mSettings );
}

void AppAndroid::privateTouchesBegan__( const TouchEvent &event )
{
	bool handled = false;
	for( CallbackMgr<bool (TouchEvent)>::iterator cbIter = mCallbacksTouchesBegan.begin(); ( cbIter != mCallbacksTouchesBegan.end() ) && ( ! handled ); ++cbIter )
		handled = (cbIter->second)( event );		
	if( ! handled )	
		touchesBegan( event );
}

void AppAndroid::privateTouchesMoved__( const TouchEvent &event )
{	
	bool handled = false;
	for( CallbackMgr<bool (TouchEvent)>::iterator cbIter = mCallbacksTouchesMoved.begin(); ( cbIter != mCallbacksTouchesMoved.end() ) && ( ! handled ); ++cbIter )
		handled = (cbIter->second)( event );		
	if( ! handled )	
		touchesMoved( event );
}

void AppAndroid::privateTouchesEnded__( const TouchEvent &event )
{	
	bool handled = false;
	for( CallbackMgr<bool (TouchEvent)>::iterator cbIter = mCallbacksTouchesEnded.begin(); ( cbIter != mCallbacksTouchesEnded.end() ) && ( ! handled ); ++cbIter )
		handled = (cbIter->second)( event );		
	if( ! handled )	
		touchesEnded( event );
}

void AppAndroid::privateAccelerated__( const Vec3f &direction )
{
	Vec3f filtered = mLastAccel * (1.0f - mAccelFilterFactor) + direction * mAccelFilterFactor;

	AccelEvent event( filtered, direction, mLastAccel, mLastRawAccel );
	
	bool handled = false;
	for( CallbackMgr<bool (AccelEvent)>::iterator cbIter = mCallbacksAccelerated.begin(); ( cbIter != mCallbacksAccelerated.end() ) && ( ! handled ); ++cbIter )
		handled = (cbIter->second)( event );		
	if( ! handled )	
		accelerated( event );

	mLastAccel = filtered;
	mLastRawAccel = direction;
}

} } // namespace cinder::app
