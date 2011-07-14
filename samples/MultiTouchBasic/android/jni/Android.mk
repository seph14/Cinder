LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cinder
LOCAL_SRC_FILES := ../../../../android/obj/local/armeabi/libcinder.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

CINDER_PATH       = $(LOCAL_PATH)/../../../..
LOCAL_MODULE     := MultiTouchBasic
LOCAL_C_INCLUDES := $(CINDER_PATH)/include \
					$(CINDER_PATH)/boost 

LOCAL_SRC_FILES := ../../src/MultiTouchApp.cpp

LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM -lz
LOCAL_STATIC_LIBRARIES := cinder android_native_app_glue 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)

