#pragma once 

#define CAVE_DECL		__declspec(dllimport)

extern "C" CAVE_DECL	void startApp();
extern "C" CAVE_DECL	void runLoop();
extern "C" CAVE_DECL	void quitApp();
extern "C" CAVE_DECL	void doFrame();
extern "C" CAVE_DECL	void doEvent(int e, int x, int y);
extern "C" CAVE_DECL	char* __XVR_INIT(void *XVR_pointer);