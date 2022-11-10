LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES  := drm_c/drm_connector.c \
					drm_c/drm_crtc.c \
					drm_c/drm_encoder.c \
					drm_c/drm_mode_fb.c \
					drm_c/drm_mode_info.c \
				  	drm_resource.c \
					show_logo_common.c \
					decompress_common.c \
					show_animation_common.c \
					charging_animation.cpp \
					utils.cpp

LOCAL_SHARED_LIBRARIES := libcutils libutils libc libstdc++ libz libdl libdrm liblog libgui libui libbase libSurfaceFlingerProp 

LOCAL_C_INCLUDES += $(TOP)/external/zlib/
LOCAL_C_INCLUDES += $(TOP)/frameworks/native/libs/nativewindow/include

LOCAL_HEADER_LIBRARIES += libdrm_headers
                    
LOCAL_MODULE := libshowlogo
LOCAL_MODULE_OWNER := mtk
LOCAL_MULTILIB := 64

include $(BUILD_SHARED_LIBRARY)
