LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm 
LOCAL_MODULE:= libid3v2
LOCAL_SRC_FILES:= \
    frame.c \
    header.c \
    id3v2lib.c \
    types.c \
    utils.c 

LOCAL_C_INCLUDES := \
    id3v2lib.h

LOCAL_CFLAGS := \
    -DHAVE_CONFIG_H \
    -DFPM_DEFAULT \
    -ffast-math \
    -O3

include $(BUILD_STATIC_LIBRARY)

