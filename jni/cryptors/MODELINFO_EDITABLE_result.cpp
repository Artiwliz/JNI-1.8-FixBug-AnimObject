#include "MODELINFO_EDITABLE_result.h"
#include "FIRST_PROTECT_result.h"
#include "protect_common.h"

#include <stdint.h>
#include "../util/armhook.h"
#include <string>
#include <sys/mman.h>
#include "../main.h"
#include "game/RW/common.h"

extern "C" void sub_20690 (uintptr_t pc)
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

	for (uintptr_t start = pc; start != pc + g_SizeStart_MODELINFO_EDITABLE; start++)
	{
		if (!memcmp((void*)start, (void*)g_Start_MODELINFO_EDITABLE, g_SizeStart_MODELINFO_EDITABLE))
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

	addr += g_SizeStart_MODELINFO_EDITABLE;

	makeNOP(addr, g_SizeCode_MODELINFO_EDITABLE / 2);

	FuckCode(pc);
	FuckCode(addr);
}