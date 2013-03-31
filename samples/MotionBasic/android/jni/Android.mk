LOCAL_PATH := $(call my-dir)/../..

include $(CLEAR_VARS)

LOCAL_MODULE     := MotionBasic

MOTION_MANAGER_PATH = ../../blocks/MotionManager

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(MOTION_MANAGER_PATH)/src
LOCAL_SRC_FILES := \
	src/MotionBasicApp.cpp \
	$(MOTION_MANAGER_PATH)/src/cinder/MotionManager.cpp \
	$(MOTION_MANAGER_PATH)/src/cinder/MotionImplAndroid.cpp

LOCAL_LDLIBS    := -landroid
LOCAL_STATIC_LIBRARIES := cinder boost_system boost_filesystem android_native_app_glue 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,cinder)
$(call import-module,boost)

