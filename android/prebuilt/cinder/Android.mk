LOCAL_PATH := $(call my-dir)

include $(LOCAL_PATH)/../../jni/cinder/Configure.mk

ifdef USE_FREEIMAGE
include $(CLEAR_VARS)
LOCAL_MODULE := freeimage
LOCAL_SRC_FILES := ../../obj/local/$(TARGET_ARCH_ABI)/libfreeimage.a
include $(PREBUILT_STATIC_LIBRARY)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := ft2
LOCAL_SRC_FILES := ../../obj/local/$(TARGET_ARCH_ABI)/libft2.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := cinder
LOCAL_SRC_FILES := ../../obj/local/$(TARGET_ARCH_ABI)/libcinder.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../jni/cinder/include $(LOCAL_PATH)/../../jni/cinder/boost
LOCAL_EXPORT_LDLIBS := -llog -lEGL -lGLESv2 -lz
include $(PREBUILT_STATIC_LIBRARY)

