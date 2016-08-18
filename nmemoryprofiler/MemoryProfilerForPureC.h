
#pragma once

void* GetMemory(unsigned int size, unsigned char dllID, char* file, int line, char* func);
void FreeMemory(void* ptr, unsigned char dllID, char* file, int line, char* func);
void* GetZeroMemory(unsigned int size, unsigned char dllID, char* file, int line, char* func);
void* ReallocMemory(void* ptr, unsigned int size, unsigned char dllID, char* file, int line, char* func);
