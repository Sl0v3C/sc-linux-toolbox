LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES:= external/tinylog/
LOCAL_SRC_FILES:= tinylog.c
LOCAL_MODULE := libtinypyylog
LOCAL_STATIC_LIBRARIES:= libcutils libutils liblog
LOCAL_CFLAGS := -Werror

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES:= external/tinylog/
LOCAL_SRC_FILES:= logpyy.c
LOCAL_MODULE := logpyy
LOCAL_STATIC_LIBRARIES:= libcutils libutils libtinypyylog liblog
LOCAL_CFLAGS := -Werror

include $(BUILD_EXECUTABLE)
