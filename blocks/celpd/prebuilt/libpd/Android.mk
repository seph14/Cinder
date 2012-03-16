LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := pdnative
LOCAL_SRC_FILES := $(TARGET_ARCH)/libpdnative.so
include $(PREBUILT_SHARED_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE := choice
# LOCAL_CFLAGS := -DPD
# LOCAL_SRC_FILES := $(TARGET_ARCH)/choice.pd_linux
# include $(PREBUILT_SHARED_LIBRARY)
# 
# include $(CLEAR_VARS)
# LOCAL_MODULE := bonk~
# LOCAL_CFLAGS := -DPD
# LOCAL_SRC_FILES := $(TARGET_ARCH)/bonk~.pd_linux
# include $(PREBUILT_SHARED_LIBRARY)
# 
# include $(CLEAR_VARS)
# LOCAL_MODULE := lrshift~
# LOCAL_CFLAGS := -DPD
# LOCAL_SRC_FILES := $(TARGET_ARCH)/lrshift~.pd_linux
# include $(PREBUILT_SHARED_LIBRARY)
# 
# include $(CLEAR_VARS)
# LOCAL_MODULE := fiddle~
# LOCAL_CFLAGS := -DPD
# LOCAL_SRC_FILES := $(TARGET_ARCH)/fiddle~.pd_linux
# include $(PREBUILT_SHARED_LIBRARY)
# 
# include $(CLEAR_VARS)
# LOCAL_MODULE := sigmund~
# LOCAL_CFLAGS := -DPD
# LOCAL_SRC_FILES := $(TARGET_ARCH)/sigmund~.pd_linux
# include $(PREBUILT_SHARED_LIBRARY)
# 
# include $(CLEAR_VARS)
# LOCAL_MODULE := pique
# LOCAL_CFLAGS := -DPD
# LOCAL_SRC_FILES := $(TARGET_ARCH)/pique.pd_linux
# include $(PREBUILT_SHARED_LIBRARY)
# 
# include $(CLEAR_VARS)
# LOCAL_MODULE := loop~
# LOCAL_CFLAGS := -DPD
# LOCAL_SRC_FILES := $(TARGET_ARCH)/loop~.pd_linux
# include $(PREBUILT_SHARED_LIBRARY)
# 
# include $(CLEAR_VARS)
# LOCAL_MODULE := expr
# LOCAL_CFLAGS := -DPD
# LOCAL_SRC_FILES := $(TARGET_ARCH)/expr.pd_linux
# include $(PREBUILT_SHARED_LIBRARY)


