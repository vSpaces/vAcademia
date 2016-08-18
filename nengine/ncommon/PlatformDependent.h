#pragma once

#include <string>
#include <windows.h>
#include <winbase.h>

std::wstring GetCurrentDirectory();
std::string GetFontsDirectory();
std::wstring GetApplicationDataDirectory();

std::wstring GetApplicationRootDirectory();
void SetApplicationRootDirectory( const std::wstring& asDirectory);
void CreateDirectory(std::wstring dir);

void MakeFullPathForUI(std::wstring& path);