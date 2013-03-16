#pragma once

#include "cinder/app/App.h"
#include "cinder/app/TouchEvent.h"
#include "cinder/app/Window.h"
// #include "cinder/app/AccelEvent.h"
#include "cinder/Filesystem.h"

#include <jni.h>
#include <time.h>

struct android_app;
struct engine;

namespace cinder { namespace app {

class AppAndroid;

enum Orientation_t
{
    ORIENTATION_ANY = 0,
    ORIENTATION_PORTRAIT,
    ORIENTATION_LANDSCAPE,
    ORIENTATION_SQUARE
};

class AppAndroid : public App
{
  public:
	class Settings : public App::Settings {
	  public:
		Settings()
			: App::Settings(), mEnableMultiTouch( true ) {}

		//! Registers the app to receive multiTouch events from the operating system. Enabled by default. If disabled, touch events are mapped to mouse events.
		void		enableMultiTouch( bool enable = true ) { mEnableMultiTouch = enable; }
		//! Returns whether the app is registered to receive multiTouch events from the operating system. Enabled by default. If disabled, touch events are mapped to mouse events.
		bool		isMultiTouchEnabled() const { return mEnableMultiTouch; }
		
	  private:
		bool		mEnableMultiTouch;
	};

	AppAndroid();
	virtual ~AppAndroid();

	virtual void		prepareSettings( Settings *settings ) {}

	/* Android activity lifecycle callbacks */

	//! Called before activity is paused
	virtual void        pause();
	//! Called when an activity resumes from a paused state
	virtual void        resume( bool renewContext );
	//! Called before an activity is destroyed
	virtual void        destroy();
	//! Called before an activity is paused, to save current state for resumption
	virtual void        setSavedState(void** savedState, size_t* size);
	//! Returns a pointer to previously saved state, or NULL if none exists
	virtual void*       getSavedState();

	//! Return read-write data path located on internal storage
	fs::path getInternalDataPath() const;
	//! Return read-write data path located on external storage
	fs::path getExternalDataPath() const;
	//! Returns Android SDK version
	int32_t  getSdkVersion();

	//! Override to respond to the beginning of a multitouch sequence
	virtual void		touchesBegan( TouchEvent event ) {}
	//! Override to respond to movement (drags) during a multitouch sequence
	virtual void		touchesMoved( TouchEvent event ) {}
	//! Override to respond to the end of a multitouch sequence
	virtual void		touchesEnded( TouchEvent event ) {}
	//! Returns a std::vector of all active touches
	const std::vector<TouchEvent::Touch>&	getActiveTouches() const { return mActiveTouches; }	
	//! Returns a Vec3d of the acceleration direction
	// virtual void		accelerated( AccelEvent event ) {}

	//! Registers a callback for touchesBegan events. Returns a unique identifier which can be used as a parameter to unregisterTouchesBegan().
	CallbackId		registerTouchesBegan( std::function<bool (TouchEvent)> callback ) { return mCallbacksTouchesBegan.registerCb( callback ); }
	//! Registers a callback for touchesBegan events. Returns a unique identifier which can be used as a parameter to unregisterTouchesBegan().
	template<typename T>
	CallbackId		registerTouchesBegan( T *obj, bool (T::*callback)(TouchEvent) ) { return mCallbacksTouchesBegan.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
	//! Unregisters a callback for touchesBegan events.
	void			unregisterTouchesBegan( CallbackId id ) { mCallbacksTouchesBegan.unregisterCb( id ); }

	//! Registers a callback for touchesMoved events. Returns a unique identifier which can be used as a parameter to unregisterTouchesMoved().
	CallbackId		registerTouchesMoved( std::function<bool (TouchEvent)> callback ) { return mCallbacksTouchesMoved.registerCb( callback ); }
	//! Registers a callback for touchesMoved events. Returns a unique identifier which can be used as a parameter to unregisterTouchesMoved().
	template<typename T>
	CallbackId		registerTouchesMoved( T *obj, bool (T::*callback)(TouchEvent) ) { return mCallbacksTouchesMoved.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
	//! Unregisters a callback for touchesMoved events.
	void			unregisterTouchesMoved( CallbackId id ) { mCallbacksTouchesMoved.unregisterCb( id ); }

	//! Registers a callback for touchesEnded events. Returns a unique identifier which can be used as a parameter to unregisterTouchesEnded().
	CallbackId		registerTouchesEnded( std::function<bool (TouchEvent)> callback ) { return mCallbacksTouchesEnded.registerCb( callback ); }
	//! Registers a callback for touchesEnded events. Returns a unique identifier which can be used as a parameter to unregisterTouchesEnded().
	template<typename T>
	CallbackId		registerTouchesEnded( T *obj, bool (T::*callback)(TouchEvent) ) { return mCallbacksTouchesEnded.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
	//! Unregisters a callback for touchesEnded events.
	void			unregisterTouchesEnded( CallbackId id ) { mCallbacksTouchesEnded.unregisterCb( id ); }

	//! Registers a callback for accelerated events. Returns a unique identifier which can be used as a parameter to unregisterAccelerated().
	// CallbackId		registerAccelerated( std::function<bool (AccelEvent)> callback ) { return mCallbacksAccelerated.registerCb( callback ); }
	// //! Registers a callback for touchesEnded events. Returns a unique identifier which can be used as a parameter to unregisterTouchesEnded().
	// template<typename T>
	// CallbackId		registerAccelerated( T *obj, bool (T::*callback)(AccelEvent) ) { return mCallbacksAccelerated.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
	// //! Unregisters a callback for touchesEnded events.
	// void			unregisterAccelerated( CallbackId id ) { mCallbacksAccelerated.unregisterCb( id ); }

	
	//! Returns the width of the App's window measured in pixels, or the screen when in full-screen mode.	
	virtual int		getWindowWidth() const;
	//! Returns the height of the App's window measured in pixels, or the screen when in full-screen mode.	
	virtual int		getWindowHeight() const;
	//! Returns the logical density of the App's window measured in dpi (dots per inch)
	virtual int		getWindowDensity() const;

	void			setWindowWidth( int windowWidth );
	void			setWindowHeight( int windowHeight );
	void			setWindowSize( int windowWidth, int windowHeight );
    void            updateWindowSizes();

	//! Enables the device's accelerometer and modifies its filtering. \a updateFrequency represents the frequency with which accelerated() is called, measured in Hz. \a filterFactor represents the amount to weight the current value relative to the previous.
	void enableAccelerometer( float updateFrequency = 30.0f, float filterFactor = 0.1f );
	//! Turns off the accelerometer
	void disableAccelerometer();
	
	//! Returns the maximum frame-rate the App will attempt to maintain.
	virtual float		getFrameRate() const;
	//! Sets the maximum frame-rate the App will attempt to maintain.
	virtual void		setFrameRate( float aFrameRate );
	//! Returns whether the App is in full-screen mode or not.
	virtual bool		isFullScreen() const;
	//! Sets whether the active App is in full-screen mode based on \a fullScreen
	virtual void		setFullScreen( bool aFullScreen );

	//! Returns the number seconds which have elapsed since the active App launched.
	virtual double		getElapsedSeconds() const;

	//! Returns the path to the application on disk
	virtual fs::path	getAppPath();

	//! Ceases execution of the application. Not implemented yet on iPhone
	virtual void	quit();

	//! Returns a pointer to the current global AppAndroid
	static AppAndroid*	get() { return static_cast<AppAndroid*>(App::get()); }

    //! Return app orientation
    Orientation_t orientation();

	//! Returns a pointer to the current global AppBasic
	virtual const Settings&	getSettings() const { return mSettings; }
    virtual WindowRef getWindow() const;
    virtual size_t getNumWindows() const;
	virtual WindowRef getWindowIndex( size_t index ) const;

    void setAndroidImpl( struct android_app* androidApp );

	//! \cond
	// These are called by application instantation macros and are only used in the launch process
	static void		prepareLaunch() { App::prepareLaunch(); }
    // XXX unify with App.h executeLaunch implementation
	static void		executeLaunch( AppAndroid* app, RendererRef defaultRenderer, const char *title )
    {
        App::sInstance = sInstance = app;
        App::executeLaunch(app, defaultRenderer, title, 0, NULL);

        // app->mDefaultRenderer = defaultRenderer;
        // CI_LOGD("Execute launch, renderer %p", defaultRenderer.get());
        // app->setAndroidImpl(androidApp);
        // App::executeLaunch(app, defaultRenderer, title, 0, NULL );
    }
	static void		cleanupLaunch() { App::cleanupLaunch(); }
	
	virtual void	launch( const char *title, int argc, char * const argv[] );
	//! \endcond

	// DO NOT CALL - should be private but aren't for esoteric reasons
	//! \cond
	// Internal handlers - these are called into by AppImpl's. If you are calling one of these, you have likely strayed far off the path.
	void		privatePrepareSettings__();
	void		privatePause__();
	void		privateResume__(bool renewContext);
	void		privateDestroy__();
	void		privateTouchesBegan__( const TouchEvent &event );
	void		privateTouchesMoved__( const TouchEvent &event );
	void		privateTouchesEnded__( const TouchEvent &event );
	void		privateSetActiveTouches__( const std::vector<TouchEvent::Touch> &touches ) { mActiveTouches = touches; }
	// void		privateAccelerated__( const Vec3f &direction );
	//! \endcond
	//

  public:
	//  JNI helpers for calling Java methods
	JavaVM* getJavaVM();
	JNIEnv* getJNIEnv();

	jclass  findClass(const char* className);
	jobject getActivity();

  protected:
	void initJNI();
	jobject   mClassLoader;
	jmethodID mFindClassMID;
    
  public:
    //  XXX not really public - shared with internal engine static methods

	//  Android Native Activity state
	struct android_app*      mAndroidApp;

    //  Window width, height
    int32_t mWidth;
    int32_t mHeight;

	struct engine* mEngine;
	
	Orientation_t orientationFromConfig();

    //  XXX creates and sets up single active window, consolidate with setup code
    void                preSetup();

  private:
	
	static AppAndroid		*sInstance;
	Settings				 mSettings;
	
	std::vector<TouchEvent::Touch>	mActiveTouches;

	CallbackMgr<bool (TouchEvent)>		mCallbacksTouchesBegan, mCallbacksTouchesMoved, mCallbacksTouchesEnded;
	// CallbackMgr<bool (AccelEvent)>		mCallbacksAccelerated;

	float					mAccelFilterFactor;
	Vec3f					mLastAccel, mLastRawAccel;

    struct timespec mStartTime;

    WindowRef createWindow( Window::Format format );
	std::list<class WindowImplAndroid*>	mWindows;
    WindowRef mActiveWindow;

  public:
	static DataSourceAssetRef loadResource(const std::string &resourcePath);

	//! Return true if a given resource is found
	static bool hasResource(const fs::path& resourcePath);
	//! Copy an Android resource (asset) to a writable path
	static void copyResource(const fs::path& resourcePath, const fs::path& destDir, bool overwrite=true);
	//! Copy an entire Android resource (asset) dir to a writable path
	static void copyResourceDir(const fs::path& resourcePath, const fs::path& destDir, bool overwrite=true);

	virtual fs::path			getAppPath() const;
};

class WindowImplAndroid
{
  public:
	WindowImplAndroid( const Window::Format &format, RendererRef sharedRenderer, AppAndroid *appImpl );

	virtual bool		isFullScreen() { return mFullScreen; }
	// virtual void		setFullScreen( bool fullScreen );
	virtual Vec2i		getSize() const { return Vec2i( mWindowWidth, mWindowHeight ); }
	// virtual void		setSize( const Vec2i &size );
	virtual Vec2i		getPos() const { return mWindowOffset; }
	// virtual void		setPos( const Vec2i &pos );
	// virtual void		close();
	// virtual std::string	getTitle() const;
	// virtual void		setTitle( const std::string &title );
	// virtual void		hide();
	// virtual void		show();
	// virtual bool		isHidden() const;
	virtual DisplayRef	getDisplay() const { return mDisplay; }
	virtual RendererRef	getRenderer() const { return mRenderer; }
	virtual const std::vector<TouchEvent::Touch>&	getActiveTouches() const { return mActiveTouches; }
	virtual void*		getNative() { return mNativeWindow; }

	// void			enableMultiTouch();
	// bool			isBorderless() const { return mBorderless; }
	// void			setBorderless( bool borderless );
	// bool			isAlwaysOnTop() const { return mAlwaysOnTop; }
	// void			setAlwaysOnTop( bool alwaysOnTop );
    void            updateWindowSize();

	AppAndroid*				getAppImpl() { return mAppImpl; }
	WindowRef				getWindow() { return mWindowRef; }
	// virtual void			keyDown( const KeyEvent &event );
	// virtual void			draw();
	// virtual void			redraw();
	// virtual void			resize();

	// void			privateClose();
  protected:
// 	void			createWindow( const Vec2i &windowSize, const std::string &title, RendererRef sharedRenderer );
// 	void			completeCreation();
// 	static void		registerWindowClass();
// 	void			getScreenSize( int clientWidth, int clientHeight, int *resultWidth, int *resultHeight );
// 	void			onTouch( HWND hWnd, WPARAM wParam, LPARAM lParam );
// 	void			toggleFullScreen();
// 
 	AppAndroid				*mAppImpl;
 	WindowRef				mWindowRef;
  	ANativeWindow			*mNativeWindow;
// 	HDC						mDC;
// 	DWORD					mWindowStyle, mWindowExStyle;
 	Vec2i					mWindowOffset;
// 	bool					mHidden;
 	int						mWindowWidth, mWindowHeight;
 	bool					mFullScreen, mBorderless, mAlwaysOnTop, mResizable;
// 	Vec2i					mWindowedPos, mWindowedSize;
 	DisplayRef				mDisplay;
 	RendererRef				mRenderer;
// 	std::map<DWORD,Vec2f>			mMultiTouchPrev;
 	std::vector<TouchEvent::Touch>	mActiveTouches;
// 	bool					mIsDragging;
// 
// 	friend AppAndroid;
// 	friend LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
};


} } // namespace cinder::app

#define CINDER_APP_ANDROID( APP, RENDERER )                                 \
extern "C" {                                                                \
  void android_main( struct android_app* state ) {                          \
    cinder::app::AppAndroid *app = new APP;                                 \
    app->setAndroidImpl(state);                                             \
    cinder::app::RendererRef ren( new RENDERER );                           \
    cinder::app::AppAndroid::executeLaunch( app, ren, #APP );               \
    cinder::app::AppAndroid::cleanupLaunch();                               \
  }                                                                         \
}

