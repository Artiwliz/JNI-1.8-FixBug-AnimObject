#pragma once

uintptr_t FindLibrary(const char* library);
void cp1251_to_utf8(char *out, const char *in, unsigned int len = 0);

void AND_OpenLink(const char* szLink);

int clampEx(int source, int min, int max);
float clampEx(float source, float min, float max);