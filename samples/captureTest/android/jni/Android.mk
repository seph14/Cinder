LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := CaptureTest

LOCAL_SRC_FILES := ../../src/captureTest.cpp

LOCAL_LDLIBS    := -landroid
LOCAL_STATIC_LIBRARIES := cinder freeimage ft2 ocvcapture_static boost_exception boost_filesystem boost_date_time boost_system android_native_app_glue 
LOCAL_SHARED_LIBRARIES := ocv_camera_r411 ocv_camera_r403 ocv_camera_r400 ocv_camera_r301 ocv_camera_r233 ocv_camera_r220 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,cinder)
$(call import-module,boost)
$(call import-module,ocvcapture)

