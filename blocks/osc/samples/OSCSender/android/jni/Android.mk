LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := oscpack
LOCAL_SRC_FILES := ../../../../lib/android/$(TARGET_ARCH_ABI)/liboscpack.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE     := OscSender

LOCAL_SRC_FILES := ../../src/OscSenderApp.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../include

LOCAL_LDLIBS    := -landroid
LOCAL_STATIC_LIBRARIES := cinder boost_system boost_filesystem boost_thread oscpack android_native_app_glue 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
$(call import-module,cinder)
$(call import-module,boost)

