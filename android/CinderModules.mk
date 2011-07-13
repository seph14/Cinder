TOP_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := cinder
LOCAL_SRC_FILES := ../../../android/obj/local/armeabi/libcinder.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := freeimage
LOCAL_SRC_FILES := ../../../android/obj/local/armeabi/libfreeimage.a
include $(PREBUILT_STATIC_LIBRARY)

