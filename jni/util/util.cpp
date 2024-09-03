#include "../main.h"
#include <vector>

uintptr_t FindLibrary(const char* library)
{
	char filename[0xFF] = { 0 },
		buffer[2048] = { 0 };
	FILE* fp = 0;
	uintptr_t address = 0;

	sprintf(filename, OBFUSCATE("/proc/%d/maps"), getpid());

	fp = fopen(filename, OBFUSCATE("rt"));
	if (fp == nullptr)
	{
		Log(OBFUSCATE("ERROR: can't open file %s"), filename);
		goto done;
	}

	while (fgets(buffer, sizeof(buffer), fp))
	{
		if (strstr(buffer, library))
		{
			address = (uintptr_t)strtoul(buffer, 0, 16);
			break;
		}
	}

done:

	if (fp)
		fclose(fp);

	return address;
}
void CrashLog(const char* fmt, ...);
#include "..//chatWindow.h"
extern CChatWindow* pChatWindow;
#include <algorithm>
#include "..//cryptors/DUMPLIBRARIES_result.h"
#include "..//str_obfuscator_no_template.hpp"

#include <unistd.h> // system api
#include <sys/mman.h>
#include <assert.h> // assert()
#include <dlfcn.h> // dlopen

auto libarm = cryptor::create(OBFUSCATE("/lib/arm/"), 10);
auto libarmeabi = cryptor::create(OBFUSCATE("/lib/armeabi-v7a"), 17);
auto packetst = cryptor::create(OBFUSCATE("com.santrope.game"), 18);
auto packetbh = cryptor::create(OBFUSCATE("com.barvikha.game"), 18);
auto procmaps = cryptor::create(OBFUSCATE("/proc/%d/maps"), 14);
#ifdef GAME_EDITION_CR
auto pmpath1 = cryptor::create(OBFUSCATE("/data/data/com.barvikha"), 24);
#else
auto pmpath1 = cryptor::create("/data/data/com.santrope", 24);
#endif
auto pmpath2 = cryptor::create(OBFUSCATE(".game/cache/libutil.so"), 23);
#include "..//cryptors/ISPMHERE_result.h"
bool IsPMHere()
{
	PROTECT_CODE_ISPMHERE;

	char path[255];
	memset(path, 0, 255);

	sprintf(path, "%s%s", pmpath1.decrypt(), pmpath2.decrypt());

	if (dlopen(path, 3))
	{
		return true;
	}

	return false;
}

bool DumpLibraries(std::vector<std::string>& buff)
{
	PROTECT_CODE_DUMPLIBRARIES;

	char filename[0xFF] = { 0 },
		buffer[2048] = { 0 };

	sprintf(filename, procmaps.decrypt(), getpid());

	FILE* fp = fopen(filename, OBFUSCATE("rt"));

	if (!fp)
	{
		return false;
	}

	while (fgets(buffer, sizeof(buffer), fp))
	{
		if (strstr(&buffer[0], packetst.decrypt()) || strstr(&buffer[0], packetbh.decrypt()))
		{
			char* pBegin = strstr(&buffer[0], libarm.decrypt());
			if (!pBegin)
			{
				pBegin = strstr(&buffer[0], libarmeabi.decrypt());
			}
			if (!pBegin)
			{
				pBegin = strstr(&buffer[0], pmpath1.decrypt());
				continue;
			}
			if (!pBegin)
			{
				continue;
			}
			char* pEnd = pBegin + strlen(pBegin) - 1;

			if (*pEnd == '\n')
			{
				*pEnd = 0;
				pEnd--;
			}

			while (*pEnd != '/')
			{
				pEnd--;
			}
			pEnd++;

			std::string toPush(pEnd);

			bool bPush = true;

			for (size_t i = 0; i < buff.size(); i++)
			{
				if (buff[i] == toPush)
				{
					bPush = false;
				}
			}

			if (bPush)
			{
				buff.push_back(toPush);
			}
		}
	}

	fclose(fp);
	return true;
}

void cp1251_to_utf8(char* out, const char* in, unsigned int len)
{
	static const int table[128] =
    {
        // 80
        0x82D0,     0x83D0,     0x9A80E2,   0x93D1,     0x9E80E2,   0xA680E2,   0xA080E2,   0xA180E2,
        0xAC82E2,   0xB080E2,   0x89D0,     0xB980E2,   0x8AD0,     0x8CD0,     0x8BD0,     0x8FD0,
        // 90
        0x92D1,     0x9880E2,   0x9980E2,   0x9C80E2,   0x9D80E2,   0xA280E2,   0x9380E2,   0x9480E2,
        0,          0xA284E2,   0x99D1,     0xBA80E2,   0x9AD1,     0x9CD1,     0x9BD1,     0x9FD1,
        // A0
        0x80b8e0,	0x81b8e0,	0x82b8e0,	0x83b8e0,	0x84b8e0,	0x85b8e0,	0x86b8e0,	0x87b8e0,
        0x88b8e0,	0x89b8e0,	0x8ab8e0,	0x8bb8e0,	0x8cb8e0,	0x8db8e0,	0x8eb8e0,	0x8fb8e0,
        // B0
        0x90b8e0,	0x91b8e0,	0x92b8e0,	0x93b8e0,	0x94b8e0,	0x95b8e0,	0x96b8e0,	0x97b8e0,
        0x98b8e0,	0x99b8e0,	0x9ab8e0,	0x9bb8e0,	0x9cb8e0,	0x9db8e0,	0x9eb8e0,	0x9fb8e0,
        // C0
        0xa0b8e0,	0xa1b8e0,	0xa2b8e0,	0xa3b8e0,	0xa4b8e0,	0xa5b8e0,	0xa6b8e0,	0xa7b8e0,
        0xa8b8e0,	0xa9b8e0,	0xaab8e0,	0xabb8e0,	0xacb8e0,	0xadb8e0,	0xaeb8e0,	0xafb8e0,
        // D0
        0xb0b8e0,	0xb1b8e0,	0xb2b8e0,	0xb3b8e0,	0xb4b8e0,	0xb5b8e0,	0xb6b8e0,	0xb7b8e0,
        0xb8b8e0,	0xb9b8e0,	0xbab8e0,	0       ,	0       ,	0       ,	0       ,	0xbfb8e0,
        // E0
        0x80b9e0,	0x81b9e0,	0x82b9e0,	0x83b9e0,	0x84b9e0,	0x85b9e0,	0x86b9e0,	0x87b9e0,
        0x88b9e0,	0x89b9e0,	0x8ab9e0,	0x8bb9e0,	0x8cb9e0,	0x8db9e0,	0x8eb9e0,	0x8fb9e0,
        // F0
        0x90b9e0,	0x91b9e0,	0x92b9e0,	0x93b9e0,	0x94b9e0,	0x95b9e0,	0x96b9e0,	0x97b9e0,
        0x98b9e0,	0x99b9e0,	0x9ab9e0,	0x9bb9e0
    };

	int count = 0;

	while (*in)
	{
		if (len && (count >= len)) break;

		if (*in & 0x80)
		{
			register int v = table[(int)(0x7f & *in++)];
			if (!v)
				continue;
			*out++ = (char)v;
			*out++ = (char)(v >> 8);
			if (v >>= 16)
				* out++ = (char)v;
		}
		else // ASCII
			*out++ = *in++;

		count++;
	}

	*out = 0;
}

void AND_OpenLink(const char* szLink)
{
	((void (*)(const char*))(SA_ADDR(0x242A64 + 1)))(szLink);
}

int clampEx(int source, int min, int max)
{
    if(source < min) return min;
    if(source > max) return max;
    return source;
}

float clampEx(float source, float min, float max)
{
    if(source < min) return min;
    if(source > max) return max;
    return source;
}