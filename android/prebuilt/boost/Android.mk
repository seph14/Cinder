LOCAL_PATH := $(call my-dir)/../../../lib/android/$(TARGET_ARCH_ABI)

include $(CLEAR_VARS)
LOCAL_MODULE := boost_chrono
LOCAL_SRC_FILES := libboost_chrono.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := boost_date_time
LOCAL_SRC_FILES := libboost_date_time.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := boost_exception
LOCAL_SRC_FILES := libboost_exception.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := boost_filesystem
LOCAL_SRC_FILES := libboost_filesystem.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := boost_random
LOCAL_SRC_FILES := libboost_random.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := boost_regex
LOCAL_SRC_FILES := libboost_regex.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := boost_signals
LOCAL_SRC_FILES := libboost_signals.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := boost_system
LOCAL_SRC_FILES := libboost_system.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := boost_thread
# LOCAL_EXPORT_CFLAGS := -DPAGE_SIZE=getpagesize\(\)
LOCAL_SRC_FILES := libboost_thread.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := boost_timer
LOCAL_SRC_FILES := libboost_timer.a
include $(PREBUILT_STATIC_LIBRARY)


