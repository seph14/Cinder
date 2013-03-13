LOCAL_PATH = $(call my-dir)/..
include $(LOCAL_PATH)/Configure.mk

APP_ABI      := 
APP_PLATFORM := android-9
APP_MODULES  := cinder
APP_CPPFLAGS := -fexceptions -frtti -Wno-format-security

ifdef USE_STL_SHARED
  APP_STL := gnustl_shared
else
  APP_STL := gnustl_static
endif

ifdef USE_CPP_11
APP_CPPFLAGS += -std=c++11 -D_LIBCPP_VERSION -D_GLIBCXX_USE_C99_MATH=1
endif

ifdef USE_ARMEABI_V5A
APP_ABI += armeabi
endif

ifdef USE_ARMEABI_V7A
APP_ABI += armeabi-v7a
endif

ifdef USE_X86
APP_ABI += x86
endif

ifdef USE_FREEIMAGE
APP_MODULES += freeimage
endif

ifdef USE_OCV_CAPTURE
APP_MODULES += ocvcapture_activity
endif

ifeq ($(NDK_TOOLCHAIN_VERSION),clang3.1)
  APP_CFLAGS += -Qunused-arguments
endif
