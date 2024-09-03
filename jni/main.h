#pragma once

#include <jni.h>
#include <typeinfo>
#include <android/log.h>
#include <ucontext.h>
#include <pthread.h>
#include <thread>
#include <malloc.h>
#include <cstdlib>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <thread>
#include <chrono>
#include <cstdarg>
#include <iterator>
#include <set>
#include <cstdint>
#include <algorithm>
#include <random>
#include <iomanip>
#include <memory>
#include <functional>
#include <map>
#include <array>
#include <sys/mman.h>
#include <unistd.h>
#include <unordered_map>
#include <dlfcn.h>
#include <dirent.h>
#include <cassert>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "game/RW/common.h"
#include "game/RW/RenderWare.h"

#include "vendor/obfuscate/str_obfuscate.hpp"

#define SAMP_VERSION	"0.3.7"
#define PORT_VERSION	"0.69"

#define MAX_PLAYERS		1004
#define MAX_VEHICLES	2000
#define MAX_PLAYER_NAME	24

#define RAKSAMP_CLIENT
#define NETCODE_CONNCOOKIELULZ 0x6969

#include "vendor/raknet/SAMP/samp_netencr.h"
#include "vendor/raknet/SAMP/SAMPRPC.h"

#include "util/util.h"

extern uintptr_t g_libGTASA;
extern const char* g_pszStorage;

void Log(const char *fmt, ...);
void LogVoice(const char *fmt, ...);

#ifdef _CDEBUG
#define DLOG(str, ...) \
	Log(str, __VA_ARGS__)
#else
#define DLOG(str, ...)
#endif

#define SA_ADDR(addr) (g_libGTASA + (addr))

uint32_t GetTickCount();


#include <audio/bass/bass.h>
#include <audio/bass/bass_fx.h>
#include <audio/opus/opus.h>
#include <audio/speex/speex.h>
#include <audio/speex/speex_echo.h>
#include <audio/speex/speex_preprocess.h>

extern int (*BASS_Init)(uint32_t, uint32_t, uint32_t);
extern int (*BASS_Free)(void);
extern int (*BASS_SetConfigPtr)(uint32_t, const char*);
extern int (*BASS_SetConfig)(uint32_t, uint32_t);
extern int (*BASS_ChannelStop)(uint32_t);
extern int (*BASS_StreamCreateURL)(char*, uint32_t, uint32_t, uint32_t);
extern int (*BASS_StreamCreate) (uint32_t, uint32_t, uint32_t, STREAMPROC *, void *);
extern int (*BASS_ChannelPlay)(uint32_t, bool);
extern int (*BASS_ChannelPause)(uint32_t);
extern int *BASS_ChannelGetTags;
extern int *BASS_ChannelSetSync;
extern int *BASS_StreamGetFilePosition;
extern int (*BASS_StreamFree)(uint32_t);
extern int (*BASS_ErrorGetCode) (void);
extern int (*BASS_Set3DFactors) (float, float, float);
extern int (*BASS_Set3DPosition) (const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *);
extern int (*BASS_Apply3D) (void);
extern int (*BASS_ChannelSetFX) (uint32_t, HFX);
extern int (*BASS_ChannelRemoveFX) (uint32_t, HFX);
extern int (*BASS_FXSetParameters) (HFX, const void *);
extern int (*BASS_IsStarted) (void);
extern int (*BASS_RecordGetDeviceInfo) (uint32_t, BASS_DEVICEINFO *);
extern int (*BASS_RecordInit) (int);
extern int (*BASS_RecordGetDevice) (void);
extern int (*BASS_RecordFree) (void);
extern int (*BASS_RecordStart) (uint32_t, uint32_t, uint32_t, RECORDPROC *, void *);
extern int (*BASS_ChannelSetAttribute) (uint32_t, uint32_t, float);
extern int (*BASS_ChannelGetData) (uint32_t, void *, uint32_t);
extern int (*BASS_RecordSetInput) (int, uint32_t, float);
extern int (*BASS_StreamPutData) (uint32_t, const void *, uint32_t);
extern int (*BASS_ChannelSetPosition) (uint32_t, uint64_t, uint32_t);
extern int (*BASS_ChannelIsActive) (uint32_t);
extern int (*BASS_ChannelSlideAttribute) (uint32_t, uint32_t, float, uint32_t);
extern int (*BASS_ChannelSet3DAttributes) (uint32_t, int, float, float, int, int, float);
extern int (*BASS_ChannelSet3DPosition) (uint32_t, const BASS_3DVECTOR *, const BASS_3DVECTOR *, const BASS_3DVECTOR *);
extern int (*BASS_SetVolume) (float);

void InitBASSFuncs();