#include "main.h"

#include "game/game.h"
#include "game/crosshair.h"
#include "game/materialtext.h"
#include "game/RW/RenderWare.h"

#include "graphics/CSkyBox.h"

#include "net/netgame.h"
#include "extrakeyboard.h"
#include "gui/gui.h"
#include "gui/CFontRenderer.h"

#include "chatwindow.h"
#include "playertags.h"
#include "dialog.h"
#include "keyboard.h"
#include "CSettings.h"
#include "CClientInfo.h"
#include "scoreboard.h"
#include "CAudioStream.h"
#include "crashlytics.h"
#include "CCheckFileHash.h"
#include "str_obfuscator_no_template.hpp"
#include "CDebugInfo.h"
#include "CServerManager.h"
#include "CLocalisation.h"
#include "KillList.h"
#include "CClientInfo.h"
#include "GButton.h"

#include "util/armhook.h"
#include "util/CStackTrace.h"
#include "util/CJavaWrapper.h"

//#include "vendor/bass/bass.h"

#include "cryptors/INITSAMP_result.h"

// voice
#include "voice/Plugin.h"

uintptr_t g_libGTASA = 0;

const char* g_pszStorage = nullptr;
const cryptor::string_encryptor encLib = cryptor::create(OBFUSCATE("libsamp.so"), 11);

CGame *pGame = nullptr;
CNetGame *pNetGame = nullptr;
CChatWindow *pChatWindow = nullptr;
CPlayerTags *pPlayerTags = nullptr;
CDialogWindow *pDialogWindow = nullptr;
CSnapShotHelper* pSnapShotHelper = nullptr;
CScoreBoard* pScoreBoard = nullptr;
CAudioStream* pAudioStream = nullptr;
CMaterialText *pMaterialText = nullptr;
CGUI *pGUI = nullptr;
CKeyBoard *pKeyBoard = nullptr;
CSettings *pSettings = nullptr;
KillList *pKillList = nullptr;
CCrossHair *pCrossHair = nullptr;
CGButton *pGButton = nullptr;
CExtraKeyBoard *pExtraKeyBoard = nullptr;

void ProcessCheckForKeyboard();
void InitHookStuff();
void InitRenderWareFunctions();
void InitSAMP(JNIEnv* pEnv, jobject thiz);
void InstallSpecialHooks();
void ApplyInGamePatches();
void ApplyPatches_level0();
void MainLoop();

#ifdef GAME_EDITION_CR
	extern uint16_t g_usLastProcessedModelIndexAutomobile;
	extern int g_iLastProcessedModelIndexAutoEnt;
#endif

extern int g_iLastProcessedSkinCollision;
extern int g_iLastProcessedEntityCollision;
extern int g_iLastRenderedObject;

extern char g_bufRenderQueueCommand[200];
extern uintptr_t g_dwRenderQueueOffset;

#define PRINT_BUILD_CRASH_INFO \
	CrashLog(OBFUSCATE("Build time: %s %s"), __TIME__, __DATE__); \
	CrashLog(OBFUSCATE("Last rendered object: %d"), g_iLastRenderedObject); \
	CrashLog(OBFUSCATE("Last processed auto and entity: %d %d"), g_usLastProcessedModelIndexAutomobile, g_iLastProcessedModelIndexAutoEnt);\
	CrashLog(OBFUSCATE("Last processed skin and entity: %d %d"), g_iLastProcessedSkinCollision, g_iLastProcessedEntityCollision);

#define PRINT_BASE_ADDRESSES \
	CrashLog(OBFUSCATE("libGTASA base address: 0x%X"), g_libGTASA); \
	CrashLog(OBFUSCATE("libsamp base address: 0x%X"), FindLibrary(OBFUSCATE("libsamp.so"))); \
	CrashLog(OBFUSCATE("libc base address: 0x%X"), FindLibrary(OBFUSCATE("libc.so")));

#define PRINT_CRASH_STATES(context) \
	CrashLog(OBFUSCATE("register states:")); \
	CrashLog(OBFUSCATE("r0: 0x%X, r1: 0x%X, r2: 0x%X, r3: 0x%X"), (context)->uc_mcontext.arm_r0, (context)->uc_mcontext.arm_r1, (context)->uc_mcontext.arm_r2, (context)->uc_mcontext.arm_r3); \
	CrashLog(OBFUSCATE("r4: 0x%x, r5: 0x%x, r6: 0x%x, r7: 0x%x"), (context)->uc_mcontext.arm_r4, (context)->uc_mcontext.arm_r5, (context)->uc_mcontext.arm_r6, (context)->uc_mcontext.arm_r7); \
	CrashLog(OBFUSCATE("r8: 0x%x, r9: 0x%x, sl: 0x%x, fp: 0x%x"), (context)->uc_mcontext.arm_r8, (context)->uc_mcontext.arm_r9, (context)->uc_mcontext.arm_r10, (context)->uc_mcontext.arm_fp); \
	CrashLog(OBFUSCATE("ip: 0x%x, sp: 0x%x, lr: 0x%x, pc: 0x%x"), (context)->uc_mcontext.arm_ip, (context)->uc_mcontext.arm_sp, (context)->uc_mcontext.arm_lr, (context)->uc_mcontext.arm_pc);

#define PRINT_BACKTRACE(context) \
	CrashLog(OBFUSCATE("backtrace:")); \
	CrashLog(OBFUSCATE("1: libGTASA.so + 0x%X"), (context)->uc_mcontext.arm_pc - g_libGTASA); \
	CrashLog(OBFUSCATE("2: libGTASA.so + 0x%X"), (context)->uc_mcontext.arm_lr - g_libGTASA); \
																					\
	CrashLog(OBFUSCATE("1: libsamp.so + 0x%X"), (context)->uc_mcontext.arm_pc - FindLibrary(OBFUSCATE("libsamp.so"))); \
	CrashLog(OBFUSCATE("2: libsamp.so + 0x%X"), (context)->uc_mcontext.arm_lr - FindLibrary(OBFUSCATE("libsamp.so"))); \
																						\
	CrashLog(OBFUSCATE("1: libc.so + 0x%X"), (context)->uc_mcontext.arm_pc - FindLibrary(OBFUSCATE("libc.so"))); \
	CrashLog(OBFUSCATE("2: libc.so + 0x%X"), (context)->uc_mcontext.arm_lr - FindLibrary(OBFUSCATE("libc.so"))); \
																							\
	PrintSymbols((void*)((context)->uc_mcontext.arm_pc), (void*)((context)->uc_mcontext.arm_lr)); \
																							\
	CStackTrace::printBacktrace(context);

extern "C"
{
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_initSAMP(JNIEnv* pEnv, jobject thiz)
	{
		InitSAMP(pEnv, thiz);
	}
}

void InitSAMP(JNIEnv* pEnv, jobject thiz)
{
	PROTECT_CODE_INITSAMP;

	Log(OBFUSCATE("Initializing SAMP.."));

	InitBASSFuncs();
	
	//BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, nullptr); // ������������� ��������� ������

	g_pszStorage = "/storage/emulated/0/Android/IRZGAMES/";

	if(!g_pszStorage)
	{
		Log(OBFUSCATE("Error: storage path not found!"));
		std::terminate();
	}

	Log(OBFUSCATE("Build time: %s %s"), __TIME__, __DATE__);
	Log(OBFUSCATE("Storage: %s"), g_pszStorage);

	pSettings = new CSettings();

	// voice
	Plugin::OnPluginLoad();
	Plugin::OnSampLoad();

	g_pJavaWrapper = new CJavaWrapper(pEnv, thiz);
	g_pJavaWrapper->SetUseFullScreen(pSettings->GetReadOnly().iCutout);

	CWeaponsOutFit::SetEnabled(pSettings->GetReadOnly().iOutfitGuns);
	CRadarRect::SetEnabled(pSettings->GetReadOnly().iRadarRect);
	CGame::SetEnabledPCMoney(pSettings->GetReadOnly().iPCMoney);
	CDebugInfo::SetDrawFPS(pSettings->GetReadOnly().iFPSCounter);
	CInfoBarText::SetEnabled(pSettings->GetReadOnly().iHPArmourText);

	CLocalisation::Initialise(OBFUSCATE("ru.lc"));
	CWeaponsOutFit::ParseDatFile();

	firebase::crashlytics::SetUserId(pSettings->GetReadOnly().szNickName);

	if(!CCheckFileHash::IsFilesValid())
	{
		CClientInfo::bSAMPModified = false;
		return;
	}
}

void InitInMenu()
{
	pGame = new CGame();
	pGame->InitInMenu();

	pGUI = new CGUI();
	pKeyBoard = new CKeyBoard();
	pChatWindow = new CChatWindow();
	pPlayerTags = new CPlayerTags();
	pDialogWindow = new CDialogWindow();
	pScoreBoard = new CScoreBoard();
	pSnapShotHelper = new CSnapShotHelper();
	pAudioStream = new CAudioStream();
    pMaterialText = new CMaterialText();
    pKillList = new KillList();
	pCrossHair = new CCrossHair();
	pGButton = new CGButton();
	pExtraKeyBoard = new CExtraKeyBoard();

	ProcessCheckForKeyboard();

	CFontRenderer::Initialise();
}

bool unique_library_handler(const char* library)
{
	Dl_info info;
	if (dladdr(__builtin_return_address(0), &info) != 0)
	{
		void* current_library_addr = dlopen(info.dli_fname, RTLD_NOW);
		if (!current_library_addr)
			return false;

		if (dlopen(library, RTLD_NOW) != current_library_addr)
			return false;
	}

	return true;
}

void ProcessCheckForKeyboard()
{
	if (pSettings->GetReadOnly().iAndroidKeyboard)
		pKeyBoard->EnableNewKeyboard();
	else pKeyBoard->EnableOldKeyboard();
}

void InitInGame()
{
	static bool bGameInited = false;
	static bool bNetworkInited = false;

	if(!bGameInited)
	{
		if (!unique_library_handler(encLib.decrypt()))
			std::terminate();

		pGame->InitInGame();
		pGame->SetMaxStats();

		pAudioStream->Initialize();

		// voice
		LogVoice("[dbg:samp:load] : module loading...");

		for(const auto& loadCallback : Samp::loadCallbacks)
		{
			if(loadCallback != nullptr)
				loadCallback();
		}

		Samp::loadStatus = true;
		
		LogVoice("[dbg:samp:load] : module loaded");

		#ifdef FLIN

		pNetGame = new CNetGame(
				g_sEncryptedAddresses[0].decrypt(),
				g_sEncryptedAddresses[0].getPort(),
				pSettings->GetReadOnly().szNickName,
				pSettings->GetReadOnly().szPassword);
	
		#endif

		bGameInited = true;
		return;
	}
	
	if(!bNetworkInited)
	{
		// HERE IS
		bNetworkInited = true;
		return;
	}
}

void MainLoop()
{
	InitInGame();

	if (pNetGame)
		pNetGame->Process();

	if (pAudioStream) 
		pAudioStream->Process();

	if (pNetGame)
	{
		if (pNetGame->GetPlayerPool())
		{
			if (pNetGame->GetPlayerPool()->GetLocalPlayer())
			{
				CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(pNetGame->GetPlayerPool()->GetLocalPlayer()->m_CurrentVehicle);
				if (pVeh)
				{
					VECTOR vec;

					pVeh->GetMoveSpeedVector(&vec);
					CDebugInfo::ProcessSpeedMode(&vec);
				}
			}
		}
	}
	else
	{
		if (pGame)
		{
			pGame->SetWorldTime(12, 0);
			pGame->DisplayWidgets(false);
		}
	}
}

void PrintSymbols(void* pc, void* lr)
{
	Dl_info info_pc, info_lr;

	if (dladdr(pc, &info_pc) != 0)
		CrashLog(OBFUSCATE("PC: %s"), info_pc.dli_sname);

	if (dladdr(lr, &info_lr) != 0)
		CrashLog(OBFUSCATE("LR: %s"), info_lr.dli_sname);
}


struct sigaction act_old;
struct sigaction act1_old;
struct sigaction act2_old;
struct sigaction act3_old;

void handler3(int signum, siginfo_t* info, void* contextPtr)
{
	auto* context = (ucontext_t*)contextPtr;

	if (act3_old.sa_sigaction)
		act3_old.sa_sigaction(signum, info, contextPtr);

	if (info->si_signo == SIGBUS)
	{
		int crashId = (int)rand() % 20000;
		Log(OBFUSCATE("Crashed. %d"), crashId);
		CrashLog(" ");

		PRINT_BUILD_CRASH_INFO;

		CrashLog(OBFUSCATE("ID: %d"), crashId);
		CrashLog(OBFUSCATE("SIGBUS | Fault address: 0x%X"), info->si_addr);

		PRINT_BASE_ADDRESSES;
		PRINT_CRASH_STATES(context);
		PRINT_BACKTRACE(context);

		exit(0);
	}
}

void handler(int signum, siginfo_t *info, void* contextPtr)
{
	auto* context = (ucontext_t*)contextPtr;

	if (act_old.sa_sigaction)
		act_old.sa_sigaction(signum, info, contextPtr);

	if(info->si_signo == SIGSEGV)
	{
		int crashId = (int)rand() % 20000;

		Log(OBFUSCATE("Crashed. %d"), crashId);
		CrashLog(" ");

		PRINT_BUILD_CRASH_INFO;

		CrashLog(OBFUSCATE("ID: %d"), crashId);
		CrashLog(OBFUSCATE("SIGSEGV | Fault address: 0x%X"), info->si_addr);

		PRINT_BASE_ADDRESSES;
		PRINT_CRASH_STATES(context);
		PRINT_BACKTRACE(context);

		exit(0);
	}
}

void handler2(int signum, siginfo_t* info, void* contextPtr)
{
	auto* context = (ucontext_t*)contextPtr;

	if (act2_old.sa_sigaction)
		act2_old.sa_sigaction(signum, info, contextPtr);

	if (info->si_signo == SIGFPE)
	{
		int crashId = (int)rand() % 20000;

		Log(OBFUSCATE("Crashed. %d"), crashId);
		CrashLog(" ");

		PRINT_BUILD_CRASH_INFO;

		CrashLog(OBFUSCATE("ID: %d"), crashId);
		CrashLog(OBFUSCATE("SIGFPE | Fault address: 0x%X"), info->si_addr);

		PRINT_BASE_ADDRESSES;
		PRINT_CRASH_STATES(context);
		PRINT_BACKTRACE(context);

		exit(0);
	}
}

void handler1(int signum, siginfo_t* info, void* contextPtr)
{
	auto* context = (ucontext_t*)contextPtr;

	if (act1_old.sa_sigaction)
		act1_old.sa_sigaction(signum, info, contextPtr);

	if (info->si_signo == SIGABRT)
	{
		int crashId = (int)rand() % 20000;

		Log(OBFUSCATE("Crashed. %d"), crashId);
		CrashLog(" ");

		PRINT_BUILD_CRASH_INFO;

		CrashLog(OBFUSCATE("ID: %d"), crashId);
		CrashLog(OBFUSCATE("SIGABRT | Fault address: 0x%X"), info->si_addr);

		PRINT_BASE_ADDRESSES;
		PRINT_CRASH_STATES(context);
		PRINT_BACKTRACE(context);

		exit(0);
	}
}

extern "C"
{
	JavaVM* javaVM = NULL;
	JavaVM* alcGetJavaVM(void) {
		return javaVM;
	}
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	javaVM = vm;

	Log(OBFUSCATE("SAMP library loaded! Build time: " __DATE__ " " __TIME__));

	g_libGTASA = FindLibrary(OBFUSCATE("libGTASA.so"));
	if(g_libGTASA == 0)
	{
		Log(OBFUSCATE("ERROR: libGTASA.so address not found!"));
		return 0;
	}

	Log(OBFUSCATE("libGTASA.so image base address: 0x%X"), g_libGTASA);

	firebase::crashlytics::Initialize();

	uintptr_t libgtasa = FindLibrary(OBFUSCATE("libGTASA.so"));
	uintptr_t libsamp = FindLibrary(OBFUSCATE("libsamp.so"));
	uintptr_t libc = FindLibrary(OBFUSCATE("libc.so"));

	Log(OBFUSCATE("libGTASA.so: 0x%x"), libgtasa);
	Log(OBFUSCATE("libsamp.so: 0x%x"), libsamp);
	Log(OBFUSCATE("libc.so: 0x%x"), libc);

	char str[100];

	sprintf(str, "0x%x", libgtasa);
	firebase::crashlytics::SetCustomKey(OBFUSCATE("libGTASA.so"), str);
	
	sprintf(str, "0x%x", libsamp);
	firebase::crashlytics::SetCustomKey(OBFUSCATE("libsamp.so"), str);

	sprintf(str, "0x%x", libc);
	firebase::crashlytics::SetCustomKey(OBFUSCATE("libc.so"), str);

	srand(time(0));

	InitHookStuff();

	InitRenderWareFunctions();
	InstallSpecialHooks();

	ApplyPatches_level0();

	struct sigaction act;
	act.sa_sigaction = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &act, &act_old);

	struct sigaction act1;
	act1.sa_sigaction = handler1;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags = SA_SIGINFO;
	sigaction(SIGABRT, &act1, &act1_old);

	struct sigaction act2;
	act2.sa_sigaction = handler2;
	sigemptyset(&act2.sa_mask);
	act2.sa_flags = SA_SIGINFO;
	sigaction(SIGFPE, &act2, &act2_old);

	struct sigaction act3;
	act3.sa_sigaction = handler3;
	sigemptyset(&act3.sa_mask);
	act3.sa_flags = SA_SIGINFO;
	sigaction(SIGBUS, &act3, &act3_old);

	return JNI_VERSION_1_4;
}

void Log(const char *fmt, ...)
{	
	char buffer[0xFF];
	static FILE* flLog = nullptr;

	if(flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, OBFUSCATE("%slogcat/samp_log.txt"), g_pszStorage);
		flLog = fopen(buffer, OBFUSCATE("w"));
	}
	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	firebase::crashlytics::Log(buffer);

	if(flLog == nullptr) return;
	fprintf(flLog, OBFUSCATE("%s\n"), buffer);
	fflush(flLog);

	__android_log_write(ANDROID_LOG_INFO, OBFUSCATE("AXL"), buffer);
}

#include "voice/header.h"
void LogVoice(const char *fmt, ...)
{
	char buffer[0xFF];
	static FILE* flLog = nullptr;

	if(flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, OBFUSCATE("%sSAMP/sampvoice/%s"), g_pszStorage, SV::kLogFileName);
		flLog = fopen(buffer, "w");
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	if(flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);
	return;
}

void CrashLog(const char* fmt, ...)
{
	char buffer[0xFF];
	static FILE* flLog = nullptr;

	if (flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, OBFUSCATE("%slogcat/crash_log.log"), g_pszStorage);
		flLog = fopen(buffer, OBFUSCATE("a"));
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	__android_log_write(ANDROID_LOG_INFO, OBFUSCATE("AXL"), buffer);

	firebase::crashlytics::Log(buffer);

	if (flLog == nullptr) return;
	fprintf(flLog, OBFUSCATE("%s\n"), buffer);
	fflush(flLog);
}

uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);

	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
