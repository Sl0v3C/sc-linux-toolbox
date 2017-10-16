LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm 
LOCAL_MODULE:= libmad
LOCAL_SRC_FILES:= \
	version.c \
	fixed.c \
	bit.c \
	timer.c \
	stream.c \
	frame.c  \
	synth.c \
	decoder.c \
	layer12.c \
	layer3.c \
	huffman.c

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/android 

LOCAL_CFLAGS := \
    -DHAVE_CONFIG_H \
    -DFPM_DEFAULT \
    -ffast-math \
    -O3

include $(BUILD_STATIC_LIBRARY)

