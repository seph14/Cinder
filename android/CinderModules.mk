# Definitions of the prebuilt static libraries exported by Cinder
# Requires the calling Android.mk to define CINDER_PATH and CINDER_MODULE_PATh

include $(CLEAR_VARS)

LOCAL_MODULE := cinder
LOCAL_SRC_FILES := $(CINDER_MODULE_PATH)/libcinder.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CINDER_PATH)/android/jni/Configure.mk

ifdef USE_FREEIMAGE
include $(CLEAR_VARS)

LOCAL_MODULE := freeimage
LOCAL_SRC_FILES := $(CINDER_MODULE_PATH)/libfreeimage.a
include $(PREBUILT_STATIC_LIBRARY)
endif

ifdef USE_FREETYPE
include $(CLEAR_VARS)

LOCAL_MODULE := ft2
LOCAL_SRC_FILES := $(CINDER_MODULE_PATH)/libft2.a
include $(PREBUILT_STATIC_LIBRARY)
endif

