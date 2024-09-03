#include "INITSAMP_result.h"
#include "FIRST_PROTECT_result.h"
#include "protect_common.h"

#include <stdint.h>
#include "../util/armhook.h"
#include <string>
#include <sys/mman.h>
#include "../main.h"
#include "game/RW/common.h"

extern "C" void sub_22263 (uintptr_t pc)
{
	static bool once = false;
	if (once)
	{
		return;
	}
	once = true;

	std::lock_guard<std::mutex> lock(g_MiscProtectMutex);

	PROTECT_CODE_FIRST_PROTECT;

	unProtect(pc);

	uintptr_t addr = 0;

	for (uintptr_t start = pc; start != pc + g_SizeStart_INITSAMP; start++)
	{
		if (!memcmp((void*)start, (void*)g_Start_INITSAMP, g_SizeStart_INITSAMP))
		{
			addr = start;
			break;
		}
	}

	if (!addr)
	{
		FuckCode(pc);
		return;
	}

	addr += g_SizeStart_INITSAMP;

	makeNOP(addr, g_SizeCode_INITSAMP / 2);

	FuckCode(pc);
	FuckCode(addr);
}