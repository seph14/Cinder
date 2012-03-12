LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := pdnative
LOCAL_SRC_FILES := $(TARGET_ARCH)/libpdnative.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := choice
LOCAL_CFLAGS := -DPD
LOCAL_SRC_FILES := $(TARGET_ARCH)/libchoice.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := bonk~
LOCAL_CFLAGS := -DPD
LOCAL_SRC_FILES := $(TARGET_ARCH)/libbonk~.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := lrshift~
LOCAL_CFLAGS := -DPD
LOCAL_SRC_FILES := $(TARGET_ARCH)/liblrshift~.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := fiddle~
LOCAL_CFLAGS := -DPD
LOCAL_SRC_FILES := $(TARGET_ARCH)/libfiddle~.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := sigmund~
LOCAL_CFLAGS := -DPD
LOCAL_SRC_FILES := $(TARGET_ARCH)/libsigmund~.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := pique
LOCAL_CFLAGS := -DPD
LOCAL_SRC_FILES := $(TARGET_ARCH)/libpique.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := loop~
LOCAL_CFLAGS := -DPD
LOCAL_SRC_FILES := $(TARGET_ARCH)/libloop~.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := expr
LOCAL_CFLAGS := -DPD
LOCAL_SRC_FILES := $(TARGET_ARCH)/libexpr.so
include $(PREBUILT_SHARED_LIBRARY)


