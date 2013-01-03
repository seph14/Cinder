LOCAL_PATH = $(call my-dir)/..
include $(LOCAL_PATH)/Configure.mk

APP_ABI      := armeabi
APP_PLATFORM := android-9
APP_STL      := gnustl_static
APP_MODULES  := cinder
APP_CPPFLAGS := -fexceptions -frtti -Wno-format-security

ifdef USE_CPP_11
APP_CPPFLAGS += -std=c++11 -D_LIBCPP_VERSION
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

ifdef USE_FREETYPE
APP_MODULES += ft2
endif

ifdef USE_OCV_CAPTURE
APP_MODULES += ocvcapture_activity
endif

