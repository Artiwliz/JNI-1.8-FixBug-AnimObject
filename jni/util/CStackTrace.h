#pragma once

#include <dlfcn.h>
#include "../main.h"

void CrashLog(const char* fmt, ...);

class CStackTrace
{
public:
	static void printBacktrace(ucontext_t *uContext)
	{
		CrashLog(OBFUSCATE("------------ START BACKTRACE ------------"));
		for (auto i = 0; i < 1000; ++i)
		{
			const auto address = *reinterpret_cast<uintptr_t*>(uContext->uc_mcontext.arm_sp + 4 * i);

			CStackTrace::printAddressBacktrace(address, (void*)(uContext->uc_mcontext.arm_pc + 4 * i), (void*)(uContext->uc_mcontext.arm_lr + 4 * i));
		}
		CrashLog(OBFUSCATE("------------ END BACKTRACE ------------"));
	}

private:
	static void printAddressBacktrace(const unsigned address, void* pc, void* lr)
	{
		Dl_info info_pc, info_lr;

		dladdr(pc, &info_pc);
		dladdr(lr, &info_lr);

		char filename[0xFF] = { 0 },
				buffer[2048] = { 0 };

		sprintf(filename, OBFUSCATE("/proc/%d/maps"), getpid());

		auto* const fp = fopen(filename, OBFUSCATE("rt"));
		if (fp == nullptr)
		{
			Log(OBFUSCATE("ERROR: can't open file %s"), filename);
			return;
		}

		while (fgets(buffer, sizeof(buffer), fp))
		{
			const auto start_address = strtoul(buffer, nullptr, 16);
			const auto end_address = strtoul(strchr(buffer, '-') + 1, nullptr, 16);

			if (start_address <= address && end_address > address)
			{
				if (*(strchr(buffer, ' ') + 3) == 'x')
				{
					CrashLog("Call: %x (GTA: %x PC: %s LR: %s) (SAMP: %x)", address, address - g_libGTASA, info_pc.dli_sname, info_lr.dli_sname, address - FindLibrary("libsamp.so"));
					break;
				}
			}
		}

		fclose(fp);
	}
};