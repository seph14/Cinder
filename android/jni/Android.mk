TOP_PATH := $(call my-dir)

include $(TOP_PATH)/../Configure.mk

ifdef USE_FREEIMAGE
  include $(TOP_PATH)/FreeImage/Android.mk
endif

ifdef USE_FREETYPE
  include $(TOP_PATH)/freetype-2.4.5/Android.mk
endif 

ifdef USE_OCV_CAPTURE
  include $(TOP_PATH)/ocvcapture/Android.mk
endif

include $(TOP_PATH)/cinder/Android.mk

