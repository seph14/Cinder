#include "cinder/app/AppAndroid.h"

#include <time.h>

#include <jni.h>
#include <errno.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#include <map>

using std::vector;
using std::map;
using ci::app::TouchEvent;

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

struct TouchState {
    vector<TouchEvent::Touch> touchesBegan;
    vector<TouchEvent::Touch> touchesMoved;
    vector<TouchEvent::Touch> touchesEnded;

    map<int32_t, TouchEvent::Touch> activeTouches;
};

struct engine {
    struct android_app* state;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

    int animating;

    int32_t width;
    int32_t height;

    struct saved_state savedState;

    struct timespec mStartTime;
    ci::app::AppAndroid* cinderApp;

    TouchState* touchState;

    //  accelerometer
    bool  accelEnabled;
    float accelUpdateFrequency;

    bool  initialized;
    bool  paused;
    bool  resumed;
    bool  renewContext;
};

static void engine_draw_frame(struct engine* engine) {
    ci::app::AppAndroid& app      = *(engine->cinderApp);
    ci::app::Renderer&   renderer = *(app.getRenderer());

    if (!renderer.isValidDisplay()) {
        // No display.
        return;
    }

    renderer.startDraw();
    app.privateUpdate__();
    app.privateDraw__();
    renderer.finishDraw();
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
    struct TouchState* touchState = engine->touchState;

    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        int32_t actionCode = AMotionEvent_getAction(event);
        int action = actionCode & AMOTION_EVENT_ACTION_MASK;
        int index  = (actionCode & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        const char* actionName = (action >= 0 && action <= 6) ? actionNames[action] : "UNKNOWN";
        CI_LOGI("Received touch action %s pointer index %d", actionName, index);

        double timestamp = engine->cinderApp->getElapsedSeconds();
        if (action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_POINTER_DOWN) {
            int pointerId = AMotionEvent_getPointerId(event, index);
            int32_t x = AMotionEvent_getX(event, index);
            int32_t y = AMotionEvent_getY(event, index);
            TouchEvent::Touch touch(ci::Vec2f(x, y), ci::Vec2f(x, y), pointerId, timestamp, NULL);
            touchState->touchesBegan.push_back(touch);
            touchState->activeTouches.insert(std::make_pair(pointerId, touch));

            CI_LOGI("Pointer id %d down x %d y %d", pointerId, x, y);
        }
        else if (action == AMOTION_EVENT_ACTION_MOVE) {
            int pointerCount = AMotionEvent_getPointerCount(event);

            for (int i=0; i < pointerCount; ++i) {
                int pointerId = AMotionEvent_getPointerId(event, i);
                int32_t x = AMotionEvent_getX(event, i);
                int32_t y = AMotionEvent_getY(event, i);
                map<int, TouchEvent::Touch>::iterator it = touchState->activeTouches.find(pointerId);
                if (it != touchState->activeTouches.end()) {
                    TouchEvent::Touch& prevTouch = it->second;
                    TouchEvent::Touch touch(ci::Vec2f(x, y), prevTouch.getPos(), pointerId, timestamp, NULL);
                    touchState->touchesMoved.push_back(touch);
                    touchState->activeTouches.erase(pointerId);
                    touchState->activeTouches.insert(std::make_pair(pointerId, touch));
                    CI_LOGI("Pointer id %d move x %d y %d", pointerId, x, y);
                }
            }
        }
        else if (action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_POINTER_UP) {
            int pointerId = AMotionEvent_getPointerId(event, index);
            int32_t x = AMotionEvent_getX(event, index);
            int32_t y = AMotionEvent_getY(event, index);
            touchState->touchesEnded.push_back(TouchEvent::Touch(ci::Vec2f(x, y), ci::Vec2f(x, y), pointerId, timestamp, NULL));
            touchState->activeTouches.erase(pointerId);
            CI_LOGI("Pointer id %d up x %d y %d", pointerId, x, y);
        }

        return 1;
    }
    return 0;
}

static void engine_update_touches(struct engine* engine) 
{
    ci::app::AppAndroid& app = *(engine->cinderApp);
    TouchState* touchState = engine->touchState;

    if ( app.getSettings().isMultiTouchEnabled() ) {
        if ( ! touchState->touchesBegan.empty() ) {
            app.privateTouchesBegan__( ci::app::TouchEvent( touchState->touchesBegan ) );
            touchState->touchesBegan.clear();
        }
        if ( ! touchState->touchesMoved.empty() ) {
            app.privateTouchesMoved__( ci::app::TouchEvent( touchState->touchesMoved ) );
            touchState->touchesMoved.clear();
        }
        if ( ! touchState->touchesEnded.empty() ) {
            app.privateTouchesEnded__( ci::app::TouchEvent( touchState->touchesEnded ) );
            touchState->touchesEnded.clear();
        }
    }
    else {
        const float contentScale = 1.0f;

        //  Mouse emulation if multi-touch is disabled
        if ( ! touchState->touchesBegan.empty() ) {
            for (vector<TouchEvent::Touch>::iterator it = touchState->touchesBegan.begin(); it != touchState->touchesBegan.end(); ++it) {
                ci::Vec2f pt = it->getPos();
                int mods = 0;
                mods |= cinder::app::MouseEvent::LEFT_DOWN;
                app.privateMouseDown__( cinder::app::MouseEvent( cinder::app::MouseEvent::LEFT_DOWN, pt.x * contentScale, pt.y * contentScale, mods, 0.0f, 0 ) );
            }
            touchState->touchesBegan.clear();
        }
        if ( ! touchState->touchesMoved.empty() ) {
            for (vector<TouchEvent::Touch>::iterator it = touchState->touchesBegan.begin(); it != touchState->touchesBegan.end(); ++it) {
                ci::Vec2f pt = it->getPos();
                int mods = 0;
                mods |= cinder::app::MouseEvent::LEFT_DOWN;
                app.privateMouseDrag__( cinder::app::MouseEvent( cinder::app::MouseEvent::LEFT_DOWN, pt.x * contentScale, pt.y * contentScale, mods, 0.0f, 0 ) );
            }
            touchState->touchesMoved.clear();
        }
        if ( ! touchState->touchesEnded.empty() ) {
            for (vector<TouchEvent::Touch>::iterator it = touchState->touchesBegan.begin(); it != touchState->touchesBegan.end(); ++it) {
                ci::Vec2f pt = it->getPos();
                int mods = 0;
                mods |= cinder::app::MouseEvent::LEFT_DOWN;
                app.privateMouseUp__( cinder::app::MouseEvent( cinder::app::MouseEvent::LEFT_DOWN, pt.x * contentScale, pt.y * contentScale, mods, 0.0f, 0 ) );
            }
            touchState->touchesEnded.clear();
        }
    }
}

inline void engine_enable_accelerometer(struct engine* engine)
{
    if (engine->accelerometerSensor != NULL) {
        ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                engine->accelerometerSensor);
        ASensorEventQueue_setEventRate(engine->sensorEventQueue,
            engine->accelerometerSensor, (1000L/engine->accelUpdateFrequency)*1000);
    }
}

inline void engine_disable_accelerometer(struct engine* engine)
{
    if (engine->accelerometerSensor != NULL) {
        ASensorEventQueue_disableSensor(engine->sensorEventQueue,
            engine->accelerometerSensor);
    }
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    ci::app::AppAndroid* cinderApp = engine->cinderApp;

    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            CI_LOGW("XXX APP_CMD_SAVE_STATE");
            // The system has asked us to save our current state.  Do so.
            engine->state->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)engine->state->savedState) = engine->savedState;
            engine->state->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            CI_LOGW("XXX APP_CMD_INIT_WINDOW");
            // The window is being shown, get it ready.
            if (engine->state->window != NULL) {
                cinderApp->getRenderer()->setup(cinderApp, engine->state->window, 
                        engine->width, engine->height);
                cinderApp->privateResize__(ci::Vec2i(engine->width, engine->height));

                if (!engine->initialized) {
                    //  First time setup
                    cinderApp->privatePrepareSettings__();
                    cinderApp->privateSetup__();
                    engine->initialized = true;
                }
                else if (engine->paused) {
                    //  Resumed and recreated context
                    engine->renewContext = true;
                }

                // engine_draw_frame(engine);
                engine->animating = 1;
            }
            break;
        case APP_CMD_TERM_WINDOW:
            CI_LOGW("XXX APP_CMD_TERM_WINDOW");
            // The window is being hidden or closed, clean it up.
            cinderApp->getRenderer()->teardown();
            engine->animating = 0;
            break;
        case APP_CMD_GAINED_FOCUS:
            CI_LOGW("XXX APP_CMD_GAINED_FOCUS");
            engine->animating = 1;

            // Start monitoring the accelerometer.
            if (engine->accelerometerSensor != NULL && engine->accelEnabled) {
                engine_enable_accelerometer(engine);
            }

            if (engine->resumed) {
                cinderApp->privateResume__(engine->renewContext);
            }

            engine->paused       = false;
            engine->resumed      = false;
            engine->renewContext = false;

            break;
        case APP_CMD_LOST_FOCUS:
            CI_LOGW("XXX APP_CMD_LOST_FOCUS");
            //  Disable accelerometer (saves power)
            engine_disable_accelerometer(engine);

            engine->animating = 0;
            engine_draw_frame(engine);
            break;
        case APP_CMD_RESUME:
            CI_LOGW("XXX APP_CMD_RESUME");
            engine->resumed = true;
            break;
        case APP_CMD_START:
            CI_LOGW("XXX APP_CMD_START");
            break;
        case APP_CMD_PAUSE:
            CI_LOGW("XXX APP_CMD_PAUSE");
            engine->paused = true;
            cinderApp->privatePause__();
            break;
        case APP_CMD_STOP:
            CI_LOGW("XXX APP_CMD_STOP");
            break;
        case APP_CMD_DESTROY:
            //  app has been destroyed, will crash if we attempt to do anything else
            CI_LOGW("XXX APP_CMD_DESTROY");
            engine->initialized = false;
            engine->paused = engine->resumed = false;
            break;
    }
}

static void android_run(struct engine* engine) 
{
    // Make sure glue isn't stripped.
    app_dummy();

    struct android_app* state = engine->state;
    state->userData     = engine;
    state->onAppCmd     = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;

    // Prepare to monitor accelerometer
    engine->sensorManager = ASensorManager_getInstance();
    engine->accelerometerSensor = ASensorManager_getDefaultSensor(engine->sensorManager,
            ASENSOR_TYPE_ACCELEROMETER);
    engine->sensorEventQueue = ASensorManager_createEventQueue(engine->sensorManager,
            state->looper, LOOPER_ID_USER, NULL, NULL);

    CI_LOGW("XXX android_run");

    if (state->savedState != NULL) {
        // We are starting with a previous saved state; restore from it.
        engine->savedState = *(struct saved_state*)state->savedState;
    }

    //  Event loop
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
                        ci::app::AppAndroid& app = *(engine->cinderApp);
                        app.privateAccelerated__(ci::Vec3f(event.acceleration.x, 
                                                           event.acceleration.y, 
                                                           event.acceleration.z));
                    }
                }
            }

            // Update input
            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                // XXX frequently crashes in teardown when orientation is locked 
                // to landscape
                // ci::app::AppAndroid& app = *(engine->cinderApp);
                // app.getRenderer()->teardown();
                engine->animating = 0;
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
    // XXX must free memory allocated for engine and touchState
    mEngine = new engine;
    memset(mEngine, 0, sizeof(engine));
    mEngine->cinderApp = this;
    mEngine->touchState = new TouchState;
    mEngine->accelEnabled = false;
	clock_gettime(CLOCK_MONOTONIC, &(mEngine->mStartTime));
	mLastAccel = mLastRawAccel = Vec3f::zero();

    //  Activity state tracking
    mEngine->initialized  = false;
    mEngine->paused       = false;
    mEngine->resumed      = false;
    mEngine->renewContext = false;
}

void AppAndroid::pause()
{
}

void AppAndroid::resume( bool renewContext )
{
    //  Override this to handle lost/recreated GL context
    //  You should recreate your GL context in this method
    if (renewContext) {
        setup();
    }
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
    if ( mEngine->accelerometerSensor != NULL ) {
	    mAccelFilterFactor = filterFactor;
	    
	    if( updateFrequency <= 0 )
	    	updateFrequency = 30.0f;

        mEngine->accelUpdateFrequency = updateFrequency;

        if ( !mEngine->accelEnabled )
            engine_enable_accelerometer(mEngine);

        mEngine->accelEnabled = true;
    }
}

void AppAndroid::disableAccelerometer() {
    if ( mEngine->accelerometerSensor != NULL && mEngine->accelEnabled ) {
        mEngine->accelEnabled = false;
        engine_disable_accelerometer(mEngine);
    }
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

void AppAndroid::privatePause__()
{
    pause();
}

void AppAndroid::privateResume__(bool renewContext) 
{
    resume(renewContext);
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

#if defined( CINDER_AASSET )
//  static loadResource method, loads from assets/
DataSourceAssetRef AppAndroid::loadResource(const std::string &resourcePath)
{
    //  XXX throw ResourceLoadExc( resourcePath ); on error
    AppAndroid* cinderApp = AppAndroid::get();
    android_app* state = cinderApp->mEngine->state;
    AAssetManager* mgr = state->activity->assetManager;
    return DataSourceAsset::createRef(mgr, resourcePath);
}
#endif

} } // namespace cinder::app
