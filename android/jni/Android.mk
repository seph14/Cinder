#  Cinder on Android makefile
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

CINDER_SRC = src/cinder

LOCAL_MODULE 	 := libcinder
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/boost
LOCAL_SRC_FILES  := $(CINDER_SRC)/Area.cpp \
                    $(CINDER_SRC)/AxisAlignedBox.cpp \
                    $(CINDER_SRC)/BandedMatrix.cpp \
                    $(CINDER_SRC)/BSpline.cpp \
                    $(CINDER_SRC)/BSplineFit.cpp \
                    $(CINDER_SRC)/Buffer.cpp \
                    $(CINDER_SRC)/Camera.cpp \
                    $(CINDER_SRC)/Color.cpp \
                    $(CINDER_SRC)/DataSource.cpp \
                    $(CINDER_SRC)/DataTarget.cpp \
                    $(CINDER_SRC)/Exception.cpp \
                    $(CINDER_SRC)/Matrix.cpp \
                    $(CINDER_SRC)/Path2D.cpp \
                    $(CINDER_SRC)/Perlin.cpp \
                    $(CINDER_SRC)/PolyLine.cpp \
                    $(CINDER_SRC)/Rand.cpp \
                    $(CINDER_SRC)/Ray.cpp \
                    $(CINDER_SRC)/Rect.cpp \
                    $(CINDER_SRC)/Shape2D.cpp \
                    $(CINDER_SRC)/Sphere.cpp \
                    $(CINDER_SRC)/Stream.cpp \
                    $(CINDER_SRC)/TriMesh.cpp \
                    $(CINDER_SRC)/Url.cpp \
                    $(CINDER_SRC)/UrlImplAndroid.cpp \
                    $(CINDER_SRC)/Utilities.cpp \
                    $(CINDER_SRC)/Xml.cpp
LOCAL_LDLIBS     := -lz

include $(BUILD_STATIC_LIBRARY)

