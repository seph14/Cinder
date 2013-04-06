LOCAL_PATH := $(call my-dir)/../..

include $(LOCAL_PATH)/Configure.mk

ifdef USE_FREEIMAGE
  include $(CLEAR_VARS)
  LOCAL_MODULE := freeimage
  LOCAL_SRC_FILES := obj/local/$(TARGET_ARCH_ABI)/libfreeimage.a
  include $(PREBUILT_STATIC_LIBRARY)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := ft2
LOCAL_SRC_FILES := ../lib/android/$(TARGET_ARCH_ABI)/libft2.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := cairo
LOCAL_SRC_FILES := ../lib/android/$(TARGET_ARCH_ABI)/libcairo.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := pixman
LOCAL_SRC_FILES := ../lib/android/$(TARGET_ARCH_ABI)/libpixman.a
include $(PREBUILT_STATIC_LIBRARY)

ifdef USE_OCV_CAPTURE
  include $(CLEAR_VARS)
  LOCAL_MODULE := ocvcapture_activity
  LOCAL_SRC_FILES := obj/local/$(TARGET_ARCH_ABI)/libocvcapture_activity.a
  include $(PREBUILT_STATIC_LIBRARY)
endif

include $(CLEAR_VARS)

include $(LOCAL_PATH)/Configure.mk
LOCAL_MODULE := cinder-es2
LOCAL_SRC_FILES := obj/local/$(TARGET_ARCH_ABI)/libcinder-es2.a
LOCAL_EXPORT_CFLAGS += -D_GLIBCPP_USE_WCHAR_T -D__LITTLE_ENDIAN__
LOCAL_EXPORT_CFLAGS += -Wno-psabi -Wno-overflow
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include $(LOCAL_PATH)/../boost
LOCAL_EXPORT_LDLIBS := -llog -lEGL -lz

ifdef USE_CPP_11
  LOCAL_EXPORT_CPPFLAGS += -std=c++11 -D_LIBCPP_VERSION -D_GLIBCXX_USE_C99_MATH=1
endif

# LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/../blocks/propipe/src
LOCAL_EXPORT_CFLAGS += -DCINDER_GLES2
LOCAL_EXPORT_LDLIBS += -lGLESv2

include $(PREBUILT_STATIC_LIBRARY)

