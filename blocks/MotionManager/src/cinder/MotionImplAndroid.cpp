#include "MotionImplAndroid.h"

#include <android_native_app_glue.h>
#include <android/sensor.h>
#include <android/log.h>

namespace cinder {

MotionImplAndroid::MotionImplAndroid()
{
    mSensorMode = MotionManager::Accelerometer;

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

}
