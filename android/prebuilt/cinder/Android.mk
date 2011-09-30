LOCAL_PATH := $(call my-dir)

include $(LOCAL_PATH)/../../jni/cinder/Configure.mk

ifdef USE_FREEIMAGE
	include $(CLEAR_VARS)
	LOCAL_MODULE := freeimage
	LOCAL_SRC_FILES := ../../obj/local/$(TARGET_ARCH_ABI)/libfreeimage.a
	include $(PREBUILT_STATIC_LIBRARY)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := ft2
LOCAL_SRC_FILES := ../../obj/local/$(TARGET_ARCH_ABI)/libft2.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := cinder
LOCAL_SRC_FILES := ../../obj/local/$(TARGET_ARCH_ABI)/libcinder.a
# required for wchar_t support, used by boost::filesystem
LOCAL_EXPORT_CFLAGS += -D_GLIBCPP_USE_WCHAR_T
ifdef USE_GLES2
	LOCAL_EXPORT_CFLAGS += -DCINDER_GLES2
	GLES_LDLIB = -lGLESv2
else
	LOCAL_EXPORT_CFLAGS += -DCINDER_GLES1
	GLES_LDLIB = -lGLESv1_CM
endif

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../include $(LOCAL_PATH)/../../../boost
LOCAL_EXPORT_LDLIBS := -llog -lEGL $(GLES_LDLIB) -lz
include $(PREBUILT_STATIC_LIBRARY)

