LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= yy_pgrep.c
LOCAL_MODULE := yypgrep
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
