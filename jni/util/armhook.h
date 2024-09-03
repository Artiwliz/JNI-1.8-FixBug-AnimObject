#pragma once

extern uintptr_t mmap_start;
extern uintptr_t mmap_end;
extern uintptr_t memlib_start;
extern uintptr_t memlib_end;

void unProtect(uintptr_t ptr, size_t dwSize = 100);
void FuckCode(uintptr_t ptr, size_t dwSize = 100);

void WriteMemory(uintptr_t dest, uintptr_t src, size_t size);
void ReadMemory(uintptr_t dest, uintptr_t src, size_t size);

void makeNOP(uintptr_t addr, unsigned int count);
void makeJMP(uintptr_t func, uintptr_t addr);
void makeBLX(uintptr_t func, uintptr_t addr);

void installHook(uintptr_t addr, uintptr_t func, uintptr_t *orig);
void InstallMethodHook(uintptr_t addr, uintptr_t func);
void installJMPHook(uintptr_t addr, uintptr_t func);
void installBLXHook(uintptr_t addr, uintptr_t func);

void CodeInject(uintptr_t addr, uintptr_t func, int register);
uintptr_t zalupa(uintptr_t func, uintptr_t addr);

template <typename Ret, typename... Args>
static Ret CallFunction(unsigned int address, Args... args)
{
	return reinterpret_cast<Ret(__cdecl *)(Args...)>(address)(args...);
}