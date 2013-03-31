TOP_PATH := $(call my-dir)

include $(TOP_PATH)/../Configure.mk

ifdef USE_FREEIMAGE
  include $(TOP_PATH)/FreeImage/Android.mk
endif

ifdef USE_OCV_CAPTURE
  include $(TOP_PATH)/ocvcapture/Android.mk
endif

USE_GLES2 =
include $(TOP_PATH)/cinder/Android.mk
USE_GLES2 = yes
include $(TOP_PATH)/cinder/Android.mk
