LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := pdnative
LOCAL_SRC_FILES := $(TARGET_ARCH)/libpdnative.so
LOCAL_EXPORT_C_INCLUDES := $(LIBPD_PATH)/libpd_wrapper $(LIBPD_PATH)/pure-data/src
include $(PREBUILT_SHARED_LIBRARY)

