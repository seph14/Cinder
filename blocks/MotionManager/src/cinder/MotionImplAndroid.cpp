#include "MotionImplAndroid.h"

#include <android_native_app_glue.h>
#include <android/sensor.h>
#include <android/log.h>

namespace cinder {

MotionImplAndroid::MotionImplAndroid()
{
    accelEnabled = false;
    mSensorMode  = MotionManager::Accelerometer;

    // Prepare to monitor accelerometer
    sensorManager = ASensorManager_getInstance();
    accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager,
            ASENSOR_TYPE_ACCELEROMETER);
    // sensorEventQueue = ASensorManager_createEventQueue(sensorManager,
    //         androidApp->looper, LOOPER_ID_USER, NULL, NULL);
}

MotionImplAndroid::~MotionImplAndroid()
{
}

bool MotionImplAndroid::isMotionUpdatesActive()
{
}

bool MotionImplAndroid::isMotionDataAvailable()
{
}

bool MotionImplAndroid::isGyroAvailable()
{
}

bool MotionImplAndroid::isAccelAvailable()
{
}

bool MotionImplAndroid::isNorthReliable()
{
}

void MotionImplAndroid::startMotionUpdates()
{
}

void MotionImplAndroid::stopMotionUpdates()
{
}

void MotionImplAndroid::setSensorMode( MotionManager::SensorMode mode )
{
}

void MotionImplAndroid::setUpdateFrequency( float updateFrequency )
{
}

void MotionImplAndroid::setShowsCalibrationView( bool shouldShow )
{
}

ci::Vec3f MotionImplAndroid::getGravityDirection( app::InterfaceOrientation orientation )
{
}

ci::Quatf MotionImplAndroid::getRotation( app::InterfaceOrientation orientation )
{
}

ci::Vec3f MotionImplAndroid::getRotationRate( app::InterfaceOrientation orientation )
{
}

ci::Vec3f MotionImplAndroid::getAcceleration( app::InterfaceOrientation orientation )
{
}

// //! Enables the accelerometer
// void enableAccelerometer( float updateFrequency, float filterFactor )
// {
//     if ( mImpl->accelerometerSensor != NULL ) {
//         mAccelFilterFactor = filterFactor;
// 
//         if( updateFrequency <= 0 )
//             updateFrequency = 30.0f;
// 
//         mImpl->accelUpdateFrequency = updateFrequency;
// 
//         if ( !mImpl->accelEnabled )
//             mImpl->enableAccelerometer();
// 
//         mImpl->accelEnabled = true;
//     }
// }
// 
// void disableAccelerometer() {
//     if ( mImpl->accelerometerSensor != NULL && mImpl->accelEnabled ) {
//         mImpl->accelEnabled = false;
//         mImpl->disableAccelerometer();
//     }
// }


void enableAccelerometer()
{
    // if (accelerometerSensor != NULL) {
    //     ASensorEventQueue_enableSensor(sensorEventQueue, accelerometerSensor);
    //     ASensorEventQueue_setEventRate(sensorEventQueue,
    //         accelerometerSensor, (1000L/accelUpdateFrequency)*1000);
    // }
}

void disableAccelerometer()
{
    // if (accelerometerSensor != NULL) {
    //     ASensorEventQueue_disableSensor(sensorEventQueue, accelerometerSensor);
    // }
}

void handleEvent()
{
    // If a sensor has data, process it now.
    // if (ident == LOOPER_ID_USER) {
    //     if (accelerometerSensor != NULL) {
    //         ASensorEvent event;
    //         while (ASensorEventQueue_getEvents(sensorEventQueue,
    //                 &event, 1) > 0) {
    //             const float kGravity = 1.0f / 9.80665f;
    //             // cinderApp->privateAccelerated__(ci::Vec3f(-event.acceleration.x * kGravity, 
    //             //                                            event.acceleration.y * kGravity, 
    //             //                                            event.acceleration.z * kGravity));
    //         }
    //     }
    // }
}

void onGainFocus()
{
    // Start monitoring the accelerometer.
    // if (accelerometerSensor != NULL && accelEnabled) {
    //     enableAccelerometer();
    // }
}

void onLostFocus()
{
    //  Disable accelerometer (saves power)
    disableAccelerometer();
}

// void AppAndroid::privateAccelerated__( const Vec3f &direction )
// {
//     Vec3f filtered = mLastAccel * (1.0f - mAccelFilterFactor) + direction * mAccelFilterFactor;
// 
//     AccelEvent event( filtered, direction, mLastAccel, mLastRawAccel );
// 
//     bool handled = false;
//     for( CallbackMgr<bool (AccelEvent)>::iterator cbIter = mCallbacksAccelerated.begin(); ( cbIter != mCallbacksAccelerated.end() ) && ( ! handled ); ++cbIter )
//         handled = (cbIter->second)( event );
//     if( ! handled )
//         accelerated( event );
// 
//     mLastAccel = filtered;
//     mLastRawAccel = direction;
// }



}
