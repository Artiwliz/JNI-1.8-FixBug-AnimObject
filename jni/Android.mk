LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libopus
LOCAL_SRC_FILES := libraries/libopus.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libspeex
LOCAL_SRC_FILES := libraries/libspeex.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := samp
LOCAL_LDLIBS := -lm -llog -lc -lz -ljnigraphics -landroid -lEGL -lGLESv2 -lOpenSLES

LOCAL_C_INCLUDES += $(wildcard $(LOCAL_PATH)/vendor/)

# samp
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/game/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/net/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/util/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/game/RW/RenderWare.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/gui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/cryptors/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/graphics/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../santrope-tea-gtasa/encryption/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../santrope-tea-gtasa/encryption/*.c)

# voice
FILE_LIST += $(wildcard $(LOCAL_PATH)/voice/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/voice/include/util/*.cpp)

# vendor
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/ini/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/inih/cpp/INIReader.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/inih/ini.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/RakNet/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/RakNet/SAMP/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/imgui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/hash/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/hash/bcrypt/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/vendor/hash/bcrypt/crypt_blowfish/*.cpp)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_STATIC_LIBRARIES := libopus libspeex
#LOCAL_CPPFLAGS := -w -s -pthread -Wall -fpack-struct=1 -O2 -std=c++14 -fexceptions
LOCAL_CPPFLAGS := -w -s -fvisibility=hidden -pthread -Wall -fpack-struct=1 -O2 -std=c++14 -fexceptions -ferror-limit=5
include $(BUILD_SHARED_LIBRARY)