
#pragma once

void DoInitializationAndLoading(HWND ahWnd);

bool DoFrame();

void DoEvent(int e, int x, int y);

void DoMainLoop();

void DoQuit();

void SetCommandLine(char* cmdLine);

void SetResourceInstance(HINSTANCE hInst);

//#define CHECK_CUR_DESKTOP_VALID 1;