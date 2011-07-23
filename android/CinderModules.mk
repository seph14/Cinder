# Definitions of the prebuilt static libraries exported by Cinder
# Requires the calling Android.mk to define CINDER_MODULE_PATh

include $(CLEAR_VARS)

LOCAL_MODULE := cinder
LOCAL_SRC_FILES := $(CINDER_MODULE_PATH)/libcinder.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := freeimage
LOCAL_SRC_FILES := $(CINDER_MODULE_PATH)/libfreeimage.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := ft2
LOCAL_SRC_FILES := $(CINDER_MODULE_PATH)/libft2.a
include $(PREBUILT_STATIC_LIBRARY)

