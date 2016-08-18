
#pragma once

#include <string>

FILE* FileOpen(const std::wstring& path, const std::wstring& mode);
FILE* FileOpenA(const std::string& path, const std::string& mode);
bool IsFileExists(const std::wstring& path);