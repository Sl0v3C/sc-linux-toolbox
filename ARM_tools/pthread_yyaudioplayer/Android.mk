LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES:= $(LOCAL_PATH) frameworks/av/media/libmedia
LOCAL_SRC_FILES:= media_player.cpp find_file.cpp yyaudioplayer.cpp
LOCAL_MODULE := yyaudioplayer
LOCAL_SHARED_LIBRARIES:= liblog libmedia libbinder libcutils libutils
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
