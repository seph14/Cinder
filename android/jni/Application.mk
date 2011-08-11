APP_ABI      := armeabi armeabi-v7a
APP_PLATFORM := android-9
APP_STL      := gnustl_static
APP_MODULES  := libcinder

MY_PATH := $(call my-dir)
include $(MY_PATH)/cinder/Configure.mk

ifdef USE_FREEIMAGE
APP_MODULES += libfreeimage
endif

ifdef USE_FREETYPE
APP_MODULES += libft2
endif

