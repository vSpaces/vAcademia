#pragma once


#ifdef WIN32
#define WS3DMAN_NO_VTABLE __declspec(novtable)
#else
#define WS3DMAN_NO_VTABLE
#endif

// defined with this macro as being exported.
#ifdef WS3DMAN_EXPORTS
#define WS3DMAN_API __declspec(dllexport)
#else
#define WS3DMAN_API __declspec(dllimport)
#endif

#include "oms_context.h"

#define WM_C3DWSRENDERSCENE WM_APP+0x1

namespace ws3d
{
	WS3DMAN_API oms::omsContext *Create3DWSManager(HWND aParent, LPRECT rect);
	WS3DMAN_API void Destroy3DWSManager(oms::omsContext *pContex);

	//////////////////////////////////////////////////////////////////////////
	// Code is written by Vladimir (2011.11.24)
	extern WS3DMAN_API HWND ghwndLauncherProcess;
	extern WS3DMAN_API int launchProcessID;
	WS3DMAN_API BOOL SendSignal2Launcher(int processId, unsigned int msgCode,unsigned int message);
	WS3DMAN_API BOOL SendData2Launcher( int processId, int msgCode, COPYDATASTRUCT* message);
	// end Vladimir (2011.11.24)
	//////////////////////////////////////////////////////////////////////////

	struct WS3DMAN_NO_VTABLE IC3DWSModule
	{
		virtual unsigned long initialize(HWND wnd) = 0;
		virtual unsigned long initResManager() = 0;
		virtual unsigned long loadUI(BSTR sSrc) = 0;
		virtual unsigned long SetLogAndTracerMode(BOOL bUseLog, int gSendLog, BOOL bUseTracer, BOOL bUseSyncLog) = 0;
		virtual unsigned long loadModule(BSTR sModuleID, BSTR sScene) = 0;
		virtual unsigned long loadScene(BSTR sSrc) = 0;
		virtual unsigned long Update(DWORD adwTime) = 0;
		virtual unsigned long getSettingValue(BSTR sName, BSTR* psVal) = 0;
		virtual unsigned long put_PathIni(BSTR psVal) = 0;

		virtual unsigned long OnLMouseDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled) = 0;
		virtual unsigned long OnRMouseDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled) = 0;
		virtual unsigned long OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled) = 0;
		virtual unsigned long OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled) = 0;
		virtual unsigned long OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled)= 0;
		virtual unsigned long OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled) = 0;
		virtual unsigned long OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled) = 0;
		virtual unsigned long OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled) = 0;
		virtual unsigned long OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled) = 0;
		virtual unsigned long OnMButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled) = 0;
		virtual unsigned long OnMButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled) = 0;
		virtual unsigned long OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled) = 0;
		virtual unsigned long OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled) = 0;
		virtual unsigned long OnCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* bHandled) = 0;
		virtual bool OnActivateApp(bool abActivated) = 0;
			// фактический размер клиентской части окна изменилс€
		virtual bool OnWindowSizeChanged(unsigned int auiWidth, unsigned int auiHeight) = 0;
		virtual void OnWindowMoving() = 0;
		
		// ”станавливает папку дл€ nengine, из которой будут грузитьс€ шейдеры и локальные текстуры
		virtual void SetApplicationRoot(LPCWSTR alpcRoot) = 0;
		virtual LPCWSTR GetApplicationRoot() = 0;

		virtual unsigned long PrepareForDestroy() = 0;
		virtual void Destroy() = 0;

		// ѕриложение пытаютс€ закрыть
			// ≈сли вернет false, то приложение не надо закрывать.
		virtual bool OnClose() = 0;

		// вызываетс€, когда на окно плеера перетащили файл
		virtual void OnFileDrop(LPCWSTR alpcFilePath) = 0;

		// вызываетс€, когда мы успешно авторизировались
		virtual void onLoginFinished( long sessionID) = 0;
		virtual long GetSessionID() = 0;
		virtual void SendMessage2Launcher( int id, unsigned long message) = 0;

		virtual bool IsExecAnySysCommand() = 0;
	};
}