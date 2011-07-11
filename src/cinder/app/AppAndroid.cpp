#include "cinder/app/AppAndroid.h"

#include <time.h>

namespace cinder { namespace app {

AppAndroid*				AppAndroid::sInstance = 0;
AppAndroid*				sInstance;

struct AppAndroidState {
// 	CinderViewCocoaTouch		*mCinderView;
// 	UIWindow					*mWindow;
	struct timespec mStartTime;
};

// void setupCocoaTouchWindow( AppAndroid *app )
// {
// 	app->privatePrepareSettings__();
// 	app->mState->mWindow = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
// 	app->mState->mCinderView = [[CinderViewCocoaTouch alloc] initWithFrame:[[UIScreen mainScreen] bounds] app:app renderer:app->getRenderer()];
// 	[app->mState->mWindow addSubview:app->mState->mCinderView];
// 	[app->mState->mCinderView release];
// 	[app->mState->mWindow makeKeyAndVisible];
// 
// 	[app->mState->mCinderView layoutIfNeeded];
// 	app->privateSetup__();
// 	[app->mState->mCinderView setAppSetupCalled:YES];
// 	app->privateResize__( ci::app::ResizeEvent( ci::Vec2i( [app->mState->mCinderView bounds].size.width, [app->mState->mCinderView bounds].size.height ) ) );
// 	
// 	[app->mState->mCinderView startAnimation];
// }

AppAndroid::AppAndroid()
	: App()
{
	mState = std::shared_ptr<AppAndroidState>( new AppAndroidState() );
	clock_gettime(CLOCK_MONOTONIC, &mState->mStartTime);
	mLastAccel = mLastRawAccel = Vec3f::zero();
}

void AppAndroid::launch( const char *title, int argc, char * const argv[] )
{
	// ::UIApplicationMain( argc, const_cast<char**>( argv ), nil, @"CinderAppDelegateIPhone" );
	// XXX Start Activity
}

int	AppAndroid::getWindowWidth() const
{
	// ::CGRect bounds = [mState->mCinderView bounds];
	// if( [mState->mCinderView respondsToSelector:NSSelectorFromString(@"contentScaleFactor")] )
	// 	return ::CGRectGetWidth( bounds ) * mState->mCinderView.contentScaleFactor;
	// else
	// 	return ::CGRectGetWidth( bounds );
}

int	AppAndroid::getWindowHeight() const
{
	// ::CGRect bounds = [mState->mCinderView bounds];
	// if( [mState->mCinderView respondsToSelector:NSSelectorFromString(@"contentScaleFactor")] )
	// 	return ::CGRectGetHeight( bounds ) * mState->mCinderView.contentScaleFactor;
	// else
	// 	return ::CGRectGetHeight( bounds );
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
			- (mState->mStartTime.tv_sec + mState->mStartTime.tv_nsec / 1e9) );
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
