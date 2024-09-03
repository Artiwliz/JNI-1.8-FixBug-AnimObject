#pragma once

#include <jni.h>

#include <string>

#define EXCEPTION_CHECK(env) \
	if ((env)->ExceptionCheck()) \ 
	{ \
		(env)->ExceptionDescribe(); \
		(env)->ExceptionClear(); \
		return; \
	}

class CJavaWrapper
{
	jobject activity;

	jmethodID s_GetClipboardText;
	jmethodID s_CallLauncherActivity;

	jmethodID s_ShowInputLayout;
	jmethodID s_HideInputLayout;

	jmethodID s_ShowClientSettings;
	jmethodID s_SetUseFullScreen;

	jmethodID s_MakeDialog;
	
	jmethodID s_showHud;
    jmethodID s_hideHud;
	jmethodID s_updateHudInfo;
	
	jmethodID s_showTabWindow;
	jmethodID s_setTabStat;
	
	
	jmethodID s_setPauseState;

	jmethodID s_showSpeed;
    jmethodID s_hideSpeed;
	jmethodID s_updateSpeedInfo;
	
	jmethodID s_showWelcome;
	
	jmethodID s_showMenu;
	

	
	
public:
	JNIEnv* GetEnv();

	std::string GetClipboardString();
	void CallLauncherActivity(int type);

	void ShowInputLayout();
	void HideInputLayout();

	void ShowClientSettings();
	void SetUseFullScreen(int b);
	
	void MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText); // Диалоги
	
	void UpdateHudInfo(int health, int armour, int hunger, int weaponid, int ammo, int ammoinclip, int money, int wanted,int score);
	void ShowHud();
    void HideHud();

	void UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock);
	void ShowSpeed();
    void HideSpeed();
	
	void ShowMenu();
	
	void ShowTabWindow();
	void SetTabStat(int id, char* name, int score, int ping);
	

	
	
	
	void SetPauseState(bool a1);
	void ShowWelcome(bool a);

	
//	void JointClient(int joint, bool bloked);

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();
};

extern CJavaWrapper* g_pJavaWrapper;