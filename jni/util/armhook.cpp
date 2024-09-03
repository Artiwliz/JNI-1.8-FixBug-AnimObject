#include "../main.h"
#include "armhook.h"
#include <sys/mman.h>

uintptr_t mmap_start 	= 0;
uintptr_t mmap_end		= 0;
uintptr_t memlib_start	= 0;
uintptr_t memlib_end	= 0;

#define HOOK_PROC "\x01\xB4\x01\xB4\x01\x48\x01\x90\x01\xBD\x00\xBF\x00\x00\x00\x00"

#include <unistd.h> // system api
#include <sys/mman.h>
#include <assert.h> // assert()
#include <dlfcn.h> // dlopen

void unProtect(uintptr_t ptr, size_t dwSize)
{
	if (dwSize)
	{
		auto* to_page = (unsigned char*)((unsigned int)(ptr) & 0xFFFFF000);
		size_t page_size = 0;

		for (int i = 0, j = 0; i < dwSize; ++i)
		{
			page_size = j * 4096;
			if (&((unsigned char*)(ptr))[i] >= &to_page[page_size])
				++j;
		}

		mprotect(to_page, page_size, PROT_READ | PROT_WRITE | PROT_EXEC);
		return;
	}
}

void FuckCode(uintptr_t ptr, size_t dwSize)
{
	if (dwSize)
	{
		auto* to_page = (unsigned char*)((unsigned int)(ptr) & 0xFFFFF000);
		size_t page_size = 0;

		for (int i = 0, j = 0; i < dwSize; ++i)
		{
			page_size = j * 4096;
			if (&((unsigned char*)(ptr))[i] >= &to_page[page_size])
				++j;
		}
		mprotect(to_page, page_size, PROT_READ | PROT_EXEC);
		return;
	}
}

void makeNOP(uintptr_t addr, unsigned int count)
{
	unProtect(addr);

    for(uintptr_t ptr = addr; ptr != (addr+(count*2)); ptr += 2)
    {
        *(char*)ptr = 0x00;
        *(char*)(ptr+1) = 0x46;
    }

    cacheflush(addr, (uintptr_t)(addr + count*2), 0);
}

void WriteMemory(uintptr_t dest, uintptr_t src, size_t size)
{
	unProtect(dest);
	memcpy((void*)dest, (void*)src, size);
	cacheflush(dest, dest+size, 0);
}
void ReadMemory(uintptr_t dest, uintptr_t src, size_t size)
{
	unProtect(src);
    memcpy((void*)dest, (void*)src, size);
}

void InitHookStuff()
{
    Log(OBFUSCATE("Initializing hook system.."));
	memlib_start = SA_ADDR(0x180694);
	memlib_end = memlib_start + 0x1A36;

	mmap_start = (uintptr_t)mmap(0, PAGE_SIZE, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	mprotect((void*)(mmap_start & 0xFFFFF000), PAGE_SIZE, PROT_READ | PROT_EXEC | PROT_WRITE);
	mmap_end = (mmap_start + PAGE_SIZE);

	unProtect(SA_ADDR(0x1800AC));

	for (int i = 0; i < 17; i++)
		LIB_CRASH_OFFSET(i, 0);
}

void makeJMP(uintptr_t func, uintptr_t addr)
{
	uint32_t code = ((addr - func - 4) >> 12) & 0x7FF | 0xF000 | ((((addr - func - 4) >> 1) & 0x7FF | 0xB800) << 16);
    WriteMemory(func, (uintptr_t)&code, 4);
}

void makeBLX(uintptr_t func, uintptr_t addr)
{
	uint32_t code = ((addr - func - 4) >> 12) & 0x7FF | ((addr - func - 4) << 15) | 0xF800F000;
	WriteMemory(func, (uintptr_t)&code, 4);
}

void WriteHookProc(uintptr_t addr, uintptr_t func)
{
	char code[16];

	memcpy(code, HOOK_PROC, 16);
	*(uint32_t*)&code[12] = (func | 1);

	WriteMemory(addr, (uintptr_t)code, 16);
}

void installBLXHook(uintptr_t addr, uintptr_t func)
{
	Log(OBFUSCATE("installJMPHook: 0x%X -> 0x%X"), addr, func);

	if(memlib_end < (memlib_start + 0x10) || mmap_end < (mmap_start + 0x20))
	{
		Log(OBFUSCATE("installBLXHook: space limit reached"));
		std::terminate();
	}

	makeBLX(addr, func);
	WriteHookProc(addr, func);

	memlib_start += 16;
}

void installJMPHook(uintptr_t addr, uintptr_t func)
{
	Log(OBFUSCATE("installJMPHook: 0x%X -> 0x%X"), addr, func);

	if(memlib_end < (memlib_start + 0x10) || mmap_end < (mmap_start + 0x20))
	{
		Log(OBFUSCATE("installJMPHook: space limit reached"));
		std::terminate();
	}

	makeJMP(addr, func);
	WriteHookProc(addr, func);

	memlib_start += 16;
}

void installHook(uintptr_t addr, uintptr_t func, uintptr_t *orig)
{
	Log(OBFUSCATE("installHook: 0x%X -> 0x%X"), addr, func);

    if(memlib_end < (memlib_start + 0x10) || mmap_end < (mmap_start + 0x20))
    {
        Log(OBFUSCATE("installHook: space limit reached"));
        std::terminate();
    }

    ReadMemory(mmap_start, addr, 4);
    WriteHookProc(mmap_start+4, addr+4);
    *orig = mmap_start+1;
    mmap_start += 32;

	makeJMP(addr, memlib_start);
    WriteHookProc(memlib_start, func);
    memlib_start += 16;
}

void RedirectCall(uintptr_t addr, uintptr_t func)
{
	makeJMP(addr, memlib_start);
	WriteHookProc(memlib_start, func);
	memlib_start += 16;
}

#pragma optimize( "", off )
#include "..///..//santrope-tea-gtasa/encryption/common.h"
int __attribute__((noinline)) g_unobfuscate1(int a)
{
	return UNOBFUSCATE_DATA(a);
}


#pragma optimize( "",  on )

void InstallMethodHook(uintptr_t addr, uintptr_t func)
{
	//testHook(g_libGTASA);

	unProtect(addr);
    *(uintptr_t*)addr = func;
}

void CodeInject(uintptr_t addr, uintptr_t func, int reg)
{
    Log(OBFUSCATE("CodeInject: 0x%X -> 0x%x (register: r%d)"), addr, func, reg);

    char injectCode[12];

    injectCode[0] = 0x01;
    injectCode[1] = 0xA0 + reg;
    injectCode[2] = (0x08 * reg) + reg;
    injectCode[3] = 0x68;
    injectCode[4] = 0x87 + (0x08 * reg);
    injectCode[5] = 0x46;
    injectCode[6] = injectCode[4];
    injectCode[7] = injectCode[5];
    
    *(uintptr_t*)&injectCode[8] = func;

    WriteMemory(addr, (uintptr_t)injectCode, 12);
}