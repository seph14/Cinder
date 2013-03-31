#pragma once

#include "cinder/Vector.h"
#include "cinder/Quaternion.h"

#include "MotionManager.h"

struct ASensor;
struct ASensorManager;
struct ASensorEventQueue;

namespace cinder {

namespace app {
  class AppAndroid;
}

class MotionImplAndroid
{
  public:
    MotionImplAndroid();
    ~MotionImplAndroid();

    bool isMotionUpdatesActive();
    bool isMotionDataAvailable();
    bool isGyroAvailable();
    bool isAccelAvailable();
    bool isNorthReliable();
    void startMotionUpdates();
    void stopMotionUpdates();
    void setSensorMode( MotionManager::SensorMode mode );
    MotionManager::SensorMode	getSensorMode() { return mSensorMode; }

    void setUpdateFrequency( float updateFrequency );
    void setShowsCalibrationView( bool shouldShow );

    ci::Vec3f getGravityDirection( app::InterfaceOrientation orientation );
    ci::Quatf getRotation( app::InterfaceOrientation orientation );
    ci::Vec3f getRotationRate( app::InterfaceOrientation orientation );
    ci::Vec3f getAcceleration( app::InterfaceOrientation orientation );

    float     getAccelFilter() const { return mAccelFilter; }
    void      setAccelFilter( float filtering ) { mAccelFilter = filtering; }

  private:
    bool  accelEnabled;
    //  accelerometer
    // float accelUpdateFrequency;

	MotionManager::SensorMode mSensorMode;
    app::AppAndroid*          mAppAndroid;

    ASensorManager*    sensorManager;
    ASensorEventQueue* sensorEventQueue;
    const ASensor*     accelerometerSensor;

    ci::Vec3f   mLastAccel;
    float       mAccelFilter;
};

} // namespace cinder
