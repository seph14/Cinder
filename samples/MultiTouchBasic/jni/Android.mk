LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../../../android/CinderModules.mk

include $(CLEAR_VARS)

LOCAL_MODULE     := MultiTouchBasic
LOCAL_C_INCLUDES := $(TOP_PATH)/jni/include \
					$(TOP_PATH)/jni/boost 

LOCAL_SRC_FILES := ../src/MultiTouchApp.cpp

LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM 
LOCAL_STATIC_LIBRARIES := android_native_app_glue cinder freeimage

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
