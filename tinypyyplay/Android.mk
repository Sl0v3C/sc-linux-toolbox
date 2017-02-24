LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES:= external/tinyalsa/include
LOCAL_SRC_FILES:= mixer.c pcm.c
LOCAL_MODULE := libtinyalsa
LOCAL_SHARED_LIBRARIES:= libcutils libutils
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES:= external/tinypyyplayer/include 
LOCAL_SRC_FILES:= mp3Parse.c
LOCAL_MODULE := mp3Parse
LOCAL_STATIC_LIBRARIES:= libcutils libutils libid3v2
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES:= external/tinypyyplayer/include
LOCAL_SRC_FILES:= pcmplay.c
LOCAL_MODULE := pcmplay
LOCAL_STATIC_LIBRARIES:= libcutils libutils libtinyalsa
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES:= external/tinypyyplayer/external-libmad/ \
                   external/tinypyyplayer/include
LOCAL_SRC_FILES:= mp3Topcm.c
LOCAL_MODULE := mp3Topcm
LOCAL_STATIC_LIBRARIES:= libcutils libutils libmad
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES:= external/tinypyyplayer/external-libmad/ \
                   external/tinypyyplayer/include
LOCAL_SRC_FILES:= tinypyyplay_fifo.c
LOCAL_MODULE := tinypyyplay_fifo
LOCAL_STATIC_LIBRARIES:= libcutils libutils libmad libtinyalsa libid3v2
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES:= external/tinypyyplayer/external-libmad/ \
                   external/tinypyyplayer/include
LOCAL_SRC_FILES:= tinypyyplay.c
LOCAL_MODULE := tinypyyplay
LOCAL_STATIC_LIBRARIES:= libcutils libutils libmad libtinyalsa libid3v2
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

include $(LOCAL_PATH)/external-libmad/Android.mk
include $(LOCAL_PATH)/id3v2lib-master/Android.mk
