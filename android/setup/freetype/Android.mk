# based on Android.mk from Android source distribution
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# compile in ARM mode, since the glyph loader/renderer is a hotspot
# when loading complex pages in the browser
#
LOCAL_ARM_MODE := arm

FREETYPE_SRC = src

LOCAL_SRC_FILES := \
	$(FREETYPE_SRC)/base/ftbbox.c \
	$(FREETYPE_SRC)/base/ftbitmap.c \
	$(FREETYPE_SRC)/base/ftglyph.c \
	$(FREETYPE_SRC)/base/ftstroke.c \
	$(FREETYPE_SRC)/base/fttype1.c \
	$(FREETYPE_SRC)/base/ftxf86.c \
	$(FREETYPE_SRC)/base/ftbase.c \
	$(FREETYPE_SRC)/base/ftsystem.c \
	$(FREETYPE_SRC)/base/ftinit.c \
	$(FREETYPE_SRC)/base/ftgasp.c \
	$(FREETYPE_SRC)/raster/raster.c \
	$(FREETYPE_SRC)/sfnt/sfnt.c \
	$(FREETYPE_SRC)/smooth/smooth.c \
	$(FREETYPE_SRC)/autofit/autofit.c \
	$(FREETYPE_SRC)/truetype/truetype.c \
	$(FREETYPE_SRC)/cff/cff.c \
	$(FREETYPE_SRC)/psnames/psnames.c \
	$(FREETYPE_SRC)/pshinter/pshinter.c

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/builds \
	$(LOCAL_PATH)/include

LOCAL_CFLAGS += -W -Wall
LOCAL_CFLAGS += -fPIC -DPIC
LOCAL_CFLAGS += "-DDARWIN_NO_CARBON"
LOCAL_CFLAGS += "-DFT2_BUILD_LIBRARY"

LOCAL_CFLAGS += -O2

LOCAL_MODULE:= ft2

include $(BUILD_STATIC_LIBRARY)

